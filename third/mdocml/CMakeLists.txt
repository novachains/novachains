LIST( APPEND SRC_FILES
      "src/att.c"
      "src/chars.c"
      "src/compat_err.c"
      "src/compat_fts.c"
      "src/compat_getline.c"
      "src/compat_getsubopt.c"
      "src/compat_isblank.c"
      "src/compat_mkdtemp.c"
      "src/compat_ohash.c"
      "src/compat_progname.c"
      "src/compat_reallocarray.c"
      "src/compat_sqlite3_errstr.c"
      "src/compat_strcasestr.c"
      "src/compat_stringlist.c"
      "src/compat_strlcat.c"
      "src/compat_strlcpy.c"
      "src/compat_strsep.c"
      "src/compat_strtonum.c"
      "src/compat_vasprintf.c"
      "src/eqn.c"
      "src/eqn_html.c"
      "src/eqn_term.c"
      "src/html.c"
      "src/lib.c"
      "src/man.c"
      "src/man_hash.c"
      "src/man_html.c"
      "src/man_macro.c"
      "src/man_term.c"
      "src/man_validate.c"
      "src/mandoc.c"
      "src/mandoc_aux.c"
      "src/mandoc_ohash.c"
      "src/manpath.c"
      "src/mdoc.c"
      "src/mdoc_argv.c"
      "src/mdoc_hash.c"
      "src/mdoc_html.c"
      "src/mdoc_macro.c"
      "src/mdoc_man.c"
      "src/mdoc_state.c"
      "src/mdoc_term.c"
      "src/msec.c"
      "src/out.c"
      "src/preconv.c"
      "src/read.c"
      "src/roff.c"
      "src/st.c"
      "src/tbl.c"
      "src/tbl_data.c"
      "src/tbl_html.c"
      "src/tbl_layout.c"
      "src/tbl_opts.c"
      "src/tbl_term.c"
      "src/term.c"
      "src/term_ascii.c"
      "src/term_ps.c"
      "src/tree.c"
      "src/common.c"
      "src/tag.c"
	  "src/parseMandoc.c"
	 )
ADD_LIBRARY( mdocml STATIC ${SRC_FILES} )
