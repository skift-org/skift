ICONS += \
	folder-home \
	folder \
	folder-text \
	folder-download \
	folder-music \
	folder-image \
	folder-heart \
	folder-cog

ICONS_SVGs = $(patsubst %, icons/%.svg, $(ICONS))
ICONS_AT_18PX = $(patsubst icons/%.svg, $(SYSROOT)/Files/Icons/%@18px.png, $(ICONS_SVGs))
ICONS_AT_24PX = $(patsubst icons/%.svg, $(SYSROOT)/Files/Icons/%@24px.png, $(ICONS_SVGs))
ICONS_AT_36PX = $(patsubst icons/%.svg, $(SYSROOT)/Files/Icons/%@36px.png, $(ICONS_SVGs))
ICONS_AT_48PX = $(patsubst icons/%.svg, $(SYSROOT)/Files/Icons/%@48px.png, $(ICONS_SVGs))

TARGETS += $(ICONS_AT_18PX) $(ICONS_AT_24PX) $(ICONS_AT_36PX) $(ICONS_AT_48PX)

list_icons:
	@echo $(ICONS_SVGs)

$(SYSROOT)/Files/Icons/%@18px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -background none -resize 18x18 $< $@

$(SYSROOT)/Files/Icons/%@24px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -background none -resize 24x24 $< $@

$(SYSROOT)/Files/Icons/%@36px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -background none -resize 36x36 $< $@

$(SYSROOT)/Files/Icons/%@48px.png: thirdparty/icons/svg/%.svg
	$(DIRECTORY_GUARD)
	@echo [ImageMagick] $(notdir $@)
	@convert -background none -resize 48x48 $< $@
