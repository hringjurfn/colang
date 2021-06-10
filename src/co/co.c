#include "common.h"
#include "parse/parse.h"
#include "ir/ir.h"
#include "ir/irbuilder.h"
#include "util/rtimer.h"

#ifdef CO_WITH_LLVM
#include "llvm/llvm.h"
#endif

ASSUME_NONNULL_BEGIN

// co filesystem directories. init() from either env (same names) or default values
static const char* COROOT = NULL;  // directory of co itself. default: argv[0]/../..
static const char* COPATH = NULL;  // directory for user files. default: ~/.co
static const char* COCACHE = NULL; // directory for build cache. default: COPATH/cache

// rtimer helpers
#define ENABLE_RTIMER_LOGGING
#ifdef ENABLE_RTIMER_LOGGING
  #define RTIMER_INIT          RTimer rtimer_ = {0}
  #define RTIMER_START()       rtimer_start(&rtimer_)
  #define RTIMER_LOG(fmt, ...) rtimer_log(&rtimer_, fmt, ##__VA_ARGS__)
#else
  #define RTIMER_INIT          do{}while(0)
  #define RTIMER_START()       do{}while(0)
  #define RTIMER_LOG(fmt, ...) do{}while(0)
#endif


bool SourceTranspile(Source* src, FILE* f) {
  //
  // TODO
  // - Extract includes (can we use .d files generated by clang, with -MF<file> maybe?)
  //   See https://clang.llvm.org/docs/ClangCommandLineReference.html#cmdoption-clang-mg
  // - Consider imports (e.g. import "package/path")
  //   Stop-gap solution: #include <package/package.h>
  // - Optional semicolons a la Go
  // - Extract exported public API
  //
  // .h/.c idea:
  //   Infer the API and then generate output that has an ifdef so it can be included
  //   as a header or compiled alone:
  //     #ifdef _PB_AS_HEADER
  //       int twice(int x);
  //     #else
  //       int twice(int x) {
  //         return x * 2;
  //       }
  //     #endif
  //   Then in another file depending on that file:
  //     #define _PB_AS_HEADER
  //     #include "twice.pbc"
  //     #include "something_else.pbc"
  //     #undef _PB_AS_HEADER
  //     #ifdef _PB_AS_HEADER
  //       int main(int argc, const char** argv);
  //     #else
  //       int main(int argc, const char** argv) {
  //         return twice(argc);
  //       }
  //     #endif
  //
  // Another idea:
  //   Generate .h for the entire package.
  //   This is a better than individual header files but is a little trickier
  //   since stuff like this can be cyclic:
  //     struct File {
  //       struct File* next;
  //       Dir*         parent;
  //     };
  //     struct Dir {
  //       File* filelist;
  //     };
  //   File depends on Dir being defined; Dir depends on File being defined.
  //   Normally in C you manually provide a forward declaration.
  //   We can forward-declare all struct types to avoid this!
  //   Note that only structs have this issue. Functions is not an issue since
  //   we'd separate the declaration from their bodies.
  //     struct File;  // fwd declaration
  //     struct Dir;   // fwd declaration
  //     struct File {
  //       struct File* next;
  //       Dir*         parent;
  //     };
  //     struct Dir {
  //       File* filelist;
  //     };
  //   Fixed!
  //
  const char* prologue =
    "#line 1 co:prologue\n"
    "_Pragma(\"clang assume_nonnull begin\")\n"
    ;
  const char* epilogue =
    "#line 1 co:epilogue\n"
    "_Pragma(\"clang assume_nonnull end\")\n"
    ;

  fwrite(prologue, strlen(prologue), 1, f);

  // #line linenum filename
  fprintf(f, "#line 1 %s\n", src->filename);

  // body
  fwrite(src->body, src->len, 1, f);

  return fwrite(epilogue, strlen(epilogue), 1, f) > 0;
}


bool parse_source1(const Pkg* pkg, Source* src) {
  if (!SourceOpenBody(src))
    panic("SourceOpenBody %s", src->filename);

  // compute SHA-1 checksum of file contents
  SourceChecksum(src);

  // outfile = COCACHE "/" checksum ".pbc"
  const char* out_suffix = ".pbc";
  size_t i = strlen(COCACHE);
  size_t outfile_len2 = 1 + strlen(src->filename) + 1 + strlen(out_suffix) + 1;
  char* outfile = memdup2(pkg->mem, COCACHE, i, outfile_len2);
  outfile[i++] = PATH_SEPARATOR;
  sha1_fmt(&outfile[i], src->sha1);
  memcpy(&outfile[i+40], ".pbc\0", 5);
  dlog("%s -> %s", src->filename, outfile);

  // process input -> output if needed
  bool ok = true;
  auto f = fopen(outfile, "wx");
  if (!f) {
    // if file exists already the cache is up to date
    if (errno != EEXIST)
      panic("fopen \"%s\" (errno #%d %s)", outfile, errno, strerror(errno));
  } else {
    ok = SourceTranspile(src, f);
    ok = fclose(f) == 0 && ok;
  }

  return SourceCloseBody(src) && ok;
}


#ifdef DEBUG
  #define PRINT_BANNER() \
    printf("————————————————————————————————————————————————————————————————\n")
#else
  #define PRINT_BANNER() do{}while(0)
#endif


static void dump_ast(const char* message, Node* ast) {
  auto s = NodeRepr(ast, str_new(512), NodeReprTypes | NodeReprLetRefs);
  dlog("%s%s", message, s);
  str_free(s);
  PRINT_BANNER();
}


#if 1
static void dump_ir(const PosMap* posmap, const IRPkg* pkg) {
  auto s = IRReprPkgStr(pkg, posmap, str_new(512));
  s = str_appendc(s, '\n');
  fwrite(s, str_len(s), 1, stderr);
  str_free(s);
}
#endif


static void diag_handler(Diagnostic* d, void* userdata) {
  auto s = str_new(strlen(d->message) + 32);
  s = diag_fmt(s, d);
  // s[str_len(s)] = '\n'; // replace nul byte
  // fwrite(s, str_len(s) + 1, 1, stderr);
  fwrite(s, str_len(s), 1, stderr);
  str_free(s);
}


int cmd_build(int argc, const char** argv) {
  if (argc < 3) {
    errlog("missing input");
    return 1;
  }

  RTIMER_INIT;
  auto timestart = nanotime();

  Pkg pkg = {
    .mem  = MemHeap,
    .dir  = ".",
    .id   = "foo/bar",
    .name = "bar",
  };

  // make sure COCACHE exists
  if (!fs_mkdirs(MemHeap, COCACHE, 0700)) {
    errlog("failed to create directory %s", COCACHE);
    return 1;
  }

  // guess argument 1 is a directory
  pkg.dir = argv[2];
  RTIMER_START();
  if (!PkgScanSources(&pkg)) {
    if (errno != ENOTDIR)
      panic("%s (errno %d %s)", pkg.dir, errno, strerror(errno));
    // guessed wrong; it's probably a file
    errno = 0; // clear errno to make errlog messages sane
    pkg.dir = path_dir(argv[2]);
    if (!PkgAddFileSource(&pkg, argv[2]))
      panic("%s (errno %d %s)", argv[2], errno, strerror(errno));
  }
  RTIMER_LOG("find source files");

  // setup build context
  RTIMER_START();
  SymPool syms = {0};
  sympool_init(&syms, universe_syms(), MemHeap, NULL);
  Mem astmem = MemHeap; // allocate AST in global memory pool
  Build build = {0};
  build_init(&build, astmem, &syms, &pkg, diag_handler, NULL);
  // build.opt = CoOptFast;
  RTIMER_LOG("init build state");

  // setup package namespace and create package AST node
  Scope* pkgscope = ScopeNew(GetGlobalScope(), build.mem);
  Node* pkgnode = CreatePkgAST(&build, pkgscope);

  // parse source files
  RTIMER_START();
  Source* src = pkg.srclist;
  Parser parser = {0};
  while (src) {
    Node* filenode = Parse(&parser, &build, src, ParseFlagsDefault, pkgscope);
    if (!filenode)
      return 1;
    NodeArrayAppend(build.mem, &pkgnode->array.a, filenode);
    NodeTransferUnresolved(pkgnode, filenode);
    src = src->next;
  }
  RTIMER_LOG("parse");
  dump_ast("", pkgnode);
  if (build.errcount) {
    errlog("%u %s", build.errcount, build.errcount == 1 ? "error" : "errors");
    return 1;
  }

  // validate AST produced by parser
  #ifdef DEBUG
  RTIMER_START();
  bool valid = NodeValidate(&build, pkgnode);
  RTIMER_LOG("validate");
  if (!valid)
    return 1;
  dlog("AST validated OK");
  #endif

  goto end; // XXX

  // resolve identifiers if needed (note: it often is needed)
  if (NodeIsUnresolved(pkgnode)) {
    RTIMER_START();
    pkgnode = ResolveSym(&build, ParseFlagsDefault, pkgnode, pkgscope);
    RTIMER_LOG("resolve symbolic references");
    //dump_ast("", pkgnode);
    if (build.errcount) {
      errlog("%u %s", build.errcount, build.errcount == 1 ? "error" : "errors");
      return 1;
    }
    assert( ! NodeIsUnresolved(pkgnode)); // no errors should mean all resolved

    // validate AST after symbol resolution
    #ifdef DEBUG
    if (!NodeValidate(&build, pkgnode))
      return 1;
    dlog("AST validated OK");
    #endif
  }

  // resolve types
  RTIMER_START();
  pkgnode = ResolveType(&build, pkgnode);
  RTIMER_LOG("semantic analysis & type resolution");
  dump_ast("", pkgnode);
  if (build.errcount) {
    errlog("%u %s", build.errcount, build.errcount == 1 ? "error" : "errors");
    return 1;
  }

  // build IR
  #if 1
  RTIMER_START();
  IRBuilder irbuilder = {};
  IRBuilderInit(&irbuilder, &build, IRBuilderComments);
  IRBuilderAddAST(&irbuilder, pkgnode);
  RTIMER_LOG("build Co IR");
  PRINT_BANNER();
  dump_ir(&build.posmap, irbuilder.pkg);
  IRBuilderDispose(&irbuilder);
  #endif

  goto end; // XXX

  // emit target code
  #ifdef CO_WITH_LLVM
  PRINT_BANNER();
  RTIMER_START();
  if (!llvm_build_and_emit(&build, pkgnode, NULL/*target=host*/)) {
    return 1;
  }
  RTIMER_LOG("llvm total");
  #endif

  UNUSED /* label */ end:
  {
    // print how much (real) time we spent
    auto timeend = nanotime();
    char abuf[40];
    auto buflen = fmtduration(abuf, countof(abuf), timeend - timestart);
    printf("done in %.*s (real time)\n", buflen, abuf);
  }
  return 0;
}

int main_usage(const char* arg0, int exit_code) {
  fprintf(exit_code == 0 ? stdout : stderr,
    "usage: %s build <srcdir>\n"
    "       %s build <srcfile> <outfile>\n"
    "       %s help\n"
    "",
    arg0,
    arg0,
    arg0
  );
  return exit_code;
}

static bool init(const char* argv0) {
  COROOT = getenv("COROOT");
  COPATH = getenv("COPATH");
  COCACHE = getenv("COCACHE");

  if (!COROOT || strlen(COROOT) == 0) {
    // COROOT is not set; infer from argv[0]
    COROOT = NULL;
    auto dirbase = argv0;
    auto dir = strrchr(dirbase, PATH_SEPARATOR);
    if (!dir && (dirbase = getenv("_")))
      dir = strrchr(dirbase, PATH_SEPARATOR);
    if (dir) {
      char buf[512];
      auto dirlen = (size_t)dir - (size_t)dirbase;
      auto len = MIN(dirlen, sizeof(buf));
      memcpy(buf, dirbase, len + 1);
      buf[len] = '\0';
      auto pch = realpath(buf, NULL);
      if (pch)
        COROOT = path_dir(pch);
    }
    if (!COROOT) {
      errlog("unable to infer COROOT; set it in env");
      return false;
    }
  }

  if (!COPATH || strlen(COPATH) == 0)
    COPATH = path_join(user_home_dir(), ".co");

  if (!COCACHE || strlen(COCACHE) == 0)
    COCACHE = path_join(COPATH, "cache");

  dlog("COROOT=%s", COROOT);
  dlog("COPATH=%s", COPATH);
  dlog("COCACHE=%s", COCACHE);

  return true;
}

int main(int argc, const char** argv) {
  #if R_TESTING_ENABLED
  if (testing_main(1, argv) != 0)
    return 1;
  #endif

  if (!init(argv[0]))
    return 1;

  if (argc < 2)
    return main_usage(argv[0], 1);

  if (strcmp(argv[1], "build") == 0)
    return cmd_build(argc, argv);

  // help | -h* | --help
  if (strstr(argv[1], "help") || strcmp(argv[1], "-h") == 0)
    return main_usage(argv[0], 0);

  if (strlen(argv[1]) > 0 && argv[1][0] == '-')
    errlog("unknown option: %s", argv[1]);
  else
    errlog("unknown command: %s", argv[1]);
  return 1;
}

ASSUME_NONNULL_END
