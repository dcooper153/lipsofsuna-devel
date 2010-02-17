EXTRA_DIST = Doxyfile makedoc.rb

all-local:
	@if test "x$(RELPATH)" = "xyes"; then \
		if test ! -e ../build/data; then \
			echo -e "\\n *** Install data files to build/data"; \
		fi; \
		echo -e "\\n *** Play by typing build/lipsofsuna\\n"; \
	fi

clean-local:
	rm -Rf html
	rm -Rf tmp
	rm -Rf "lipsofsuna-docs-$(VERSION)"
	rm -f "lipsofsuna-docs-$(VERSION).tar.gz"

html-local:
	rm -Rf html
	rm -Rf tmp
	mkdir -p html
	mkdir -p html/api
	mkdir -p html/lua
	mkdir -p tmp
	doxygen "$(srcdir)/Doxyfile"
	ruby "$(srcdir)/makedoc.rb" "$(srcdir)/../lipsofsuna" tmp
	luadoc --nofiles -d html/lua tmp
	rm -Rf tmp
	cp -R html "lipsofsuna-docs-$(VERSION)"
	tar czf "../lipsofsuna-docs-$(VERSION).tar.gz" "lipsofsuna-docs-$(VERSION)"
	rm -Rf "lipsofsuna-docs-$(VERSION)"
