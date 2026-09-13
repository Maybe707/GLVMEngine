# A changed compiler, profile or flags invalidates existing objects, even with BUILD overridden.
.PHONY: FORCE
FORCE:
$(BUILD)/.build-config: FORCE
	@mkdir -p "$(@D)"
	@printf '%s\n' '$(CC)|$(CXX)|$(C)|$(AR)|$(CONFIG)|$(CPPFLAGS)|$(CXXFLAGS)|$(CFLAGS)|$(SANITIZE)|$(LDFLAGS)|$(LDLIBS)' > "$@.tmp"
	@cmp -s "$@.tmp" "$@" && rm "$@.tmp" || mv "$@.tmp" "$@"
$(OBJECTS) $(OBJECTS_C): $(BUILD)/.build-config
