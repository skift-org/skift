ICONS_SVGs = $(patsubst %, thirdparty/icons/svg/%.svg, $(ICONS))

ICONS_AT_18PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/System/Icons/%@18px.png, $(ICONS_SVGs))
ICONS_AT_24PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/System/Icons/%@24px.png, $(ICONS_SVGs))
ICONS_AT_36PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/System/Icons/%@36px.png, $(ICONS_SVGs))
ICONS_AT_48PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/System/Icons/%@48px.png, $(ICONS_SVGs))

TARGETS += $(ICONS_AT_18PX) $(ICONS_AT_24PX) $(ICONS_AT_36PX) $(ICONS_AT_48PX)

.PRECIOUS: cache/Icons/%@18px.png
cache/Icons/%@18px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 18 -h 18 $< || \
	 inkscape --export-png $@ -w 18 -h 18 $< &>/dev/null

.PRECIOUS: cache/Icons/%@24px.png
cache/Icons/%@24px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 24 -h 24 $< || \
	 inkscape --export-png $@ -w 24 -h 24 $< &>/dev/null

.PRECIOUS: cache/Icons/%@36px.png
cache/Icons/%@36px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 36 -h 36 $< || \
	 inkscape --export-png $@ -w 36 -h 36 $< &>/dev/null

.PRECIOUS: cache/Icons/%@48px.png
cache/Icons/%@48px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 48 -h 48 $< || \
	 inkscape --export-png $@ -w 48 -h 48 $< &>/dev/null

$(SYSROOT)/System/Icons/%@18px.png: cache/Icons/%@18px.png
	$(DIRECTORY_GUARD)
	@cp $< $@

$(SYSROOT)/System/Icons/%@24px.png: cache/Icons/%@24px.png
	$(DIRECTORY_GUARD)
	@cp $< $@

$(SYSROOT)/System/Icons/%@36px.png: cache/Icons/%@36px.png
	$(DIRECTORY_GUARD)
	@cp $< $@

$(SYSROOT)/System/Icons/%@48px.png: cache/Icons/%@48px.png
	$(DIRECTORY_GUARD)
	@cp $< $@
