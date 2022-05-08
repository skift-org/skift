PKG:=$(shell echo '$(PKG_NAME)' | tr '[:lower:]' '[:upper:]')
PKG_SRCDIR:=$(SRCDIR)/$(PKG_NAME)

PKG_BIN:=$(BINDIR)/$(PKG_NAME)
PKG_LIB:=$(BINDIR)/$(PKG_NAME).a

PKG_OBJDIR:=$(OBJDIR)/$(PKG_NAME)
PKG_SRCS:=$(wildcard $(PKG_SRCDIR)/*.cpp) $(wildcard $(PKG_SRCDIR)/*.s)
PKG_OBJS:=$(patsubst $(PKG_SRCDIR)/%,$(PKG_OBJDIR)/%.o,$(PKG_SRCS))

define PKG_META
{
    "id": "$(PKG)",

	"name": "$(PKG_NAME)",
    "deps": "$(PKG_DEPS)",

    "srcdir": "$(PKG_SRCDIR)",
    "objdir": "$(PKG_OBJ)",

	"srcs": "$(PKG_SRCS)",
	"objs": "$(PKG_OBJS)",

	"bin": "$(PKG_BIN)",
    "lib": "$(PKG_LIB)"
}
endef

META:=$(PKG_META),$(NEWLINE)$(META)

