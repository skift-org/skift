PKG:=$(shell echo '$(PKG_NAME)' | tr '[:lower:]-' '[:upper:]_')
PKG_SRCDIR:=$(SRCDIR)/$(PKG_NAME)
PKG_OBJDIR:=$(OBJDIR)/$(PKG_NAME)

PKG_BIN:=$(BINDIR)/$(PKG_NAME)
PKG_LIB:=$(BINDIR)/$(PKG_NAME).a

PKG_SRCS:=$(wildcard $(PKG_SRCDIR)/*.cpp) $(wildcard $(PKG_SRCDIR)/*.s)
PKG_OBJS:=$(patsubst $(PKG_SRCDIR)/%,$(PKG_OBJDIR)/%.o,$(PKG_SRCS))
PKG_LIBS:=$(patsubst %,$(BINDIR)/%.a,$(PKG_DEPS))

-include $(PKG_OBJS:.o=.d)

define RECIPES

$(PKG_BIN): $(PKG_OBJS)
	$$(MKCWD)
	$(CXX) -o $(PKG_BIN) $(PKG_OBJS) $(LDLIBS) $(CFLAGS) $(LDFLAGS)

$(PKG_LIB): $(PKG_OBJS)
	$$(MKCWD)
	$(AR) $(ARFLAGS) $(PKG_LIB) $(PKG_OBJS)

endef


$(eval $(call RECIPES))


define PKG_META
{
	"id": "$(PKG_NAME)",
    "deps": "$(PKG_DEPS)",

    "srcdir": "$(PKG_SRCDIR)",
    "objdir": "$(PKG_OBJDIR)",

	"srcs": "$(PKG_SRCS)",
	"objs": "$(PKG_OBJS)",
	"libs": "$(PKG_LIBS)",

	"bin": "$(PKG_BIN)",
    "lib": "$(PKG_LIB)"
}
endef

ifeq ($(PKG_TYPE),library)
	ALL_LIBS:=$(PKG_LIB) $(ALL_LIBS)
else
	ALL_BINS:=$(PKG_BIN) $(ALL_BINS)
endif

ALL_META:=$(PKG_META),$(NEWLINE)$(ALL_META)

