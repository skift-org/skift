ICONS += \
	folder-home \
	folder \
	folder-text \
	folder-download \
	folder-music \
	folder-image \
	folder-heart \
	folder-cog

ICONS_SVGs = $(patsubst %, thirdparty/icons/svg/%.svg, $(ICONS))

ICONS_AT_18PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/Files/Icons/%@18px.png, $(ICONS_SVGs))
ICONS_AT_24PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/Files/Icons/%@24px.png, $(ICONS_SVGs))
ICONS_AT_36PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/Files/Icons/%@36px.png, $(ICONS_SVGs))
ICONS_AT_48PX = $(patsubst thirdparty/icons/svg/%.svg, $(SYSROOT)/Files/Icons/%@48px.png, $(ICONS_SVGs))

TARGETS += $(ICONS_AT_18PX) $(ICONS_AT_24PX) $(ICONS_AT_36PX) $(ICONS_AT_48PX)

$(SYSROOT)/Files/Icons/%@18px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 18 -h 18 $< || \
	 inkscape --export-png $@ -w 18 -h 18 $< &>/dev/null

$(SYSROOT)/Files/Icons/%@24px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 24 -h 24 $< || \
	 inkscape --export-png $@ -w 24 -h 24 $< &>/dev/null

$(SYSROOT)/Files/Icons/%@36px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 36 -h 36 $< || \
	 inkscape --export-png $@ -w 36 -h 36 $< &>/dev/null

$(SYSROOT)/Files/Icons/%@48px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ICON] $(notdir $@)
	@inkscape --export-filename=$@ -w 48 -h 48 $< || \
	 inkscape --export-png $@ -w 48 -h 48 $< &>/dev/null
