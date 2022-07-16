#pragma once

#include <karm-gfx/color.h>

namespace Karm::Gfx {

[[gnu::used]] static constexpr Color TRANSPARENT = {0, 0, 0, 0};

[[gnu::used]] static constexpr Color BLACK = {0, 0, 0, 255};
[[gnu::used]] static constexpr Color DARK_GRAY = {64, 64, 64, 255};
[[gnu::used]] static constexpr Color GRAY = {128, 128, 128, 255};
[[gnu::used]] static constexpr Color LIGHT_GRAY = {192, 192, 192, 255};
[[gnu::used]] static constexpr Color WHITE = {255, 255, 255, 255};

[[gnu::used]] static constexpr Color RED = {255, 0, 0, 255};
[[gnu::used]] static constexpr Color GREEN = {0, 255, 0, 255};
[[gnu::used]] static constexpr Color BLUE = {0, 0, 255, 255};

[[gnu::used]] static constexpr Color YELLOW = {255, 255, 0, 255};
[[gnu::used]] static constexpr Color CYAN = {0, 255, 255, 255};
[[gnu::used]] static constexpr Color MAGENTA = {255, 0, 255, 255};

// from https://tailwindcss.com/docs/customizing-colors
[[gnu::used]] static constexpr Color SLATE50 = Color::fromHex(0xfff8fafc);
[[gnu::used]] static constexpr Color SLATE100 = Color::fromHex(0xfff1f5f9);
[[gnu::used]] static constexpr Color SLATE200 = Color::fromHex(0xffe2e8f0);
[[gnu::used]] static constexpr Color SLATE300 = Color::fromHex(0xffcbd5e1);
[[gnu::used]] static constexpr Color SLATE400 = Color::fromHex(0xff94a3b8);
[[gnu::used]] static constexpr Color SLATE500 = Color::fromHex(0xff64748b);
[[gnu::used]] static constexpr Color SLATE600 = Color::fromHex(0xff475569);
[[gnu::used]] static constexpr Color SLATE700 = Color::fromHex(0xff334155);
[[gnu::used]] static constexpr Color SLATE800 = Color::fromHex(0xff1e293b);
[[gnu::used]] static constexpr Color SLATE900 = Color::fromHex(0xff0f172a);

[[gnu::used]] static constexpr Color GRAY50 = Color::fromHex(0xfff9fafb);
[[gnu::used]] static constexpr Color GRAY100 = Color::fromHex(0xfff3f4f6);
[[gnu::used]] static constexpr Color GRAY200 = Color::fromHex(0xffe5e7eb);
[[gnu::used]] static constexpr Color GRAY300 = Color::fromHex(0xffd1d5db);
[[gnu::used]] static constexpr Color GRAY400 = Color::fromHex(0xff9ca3af);
[[gnu::used]] static constexpr Color GRAY500 = Color::fromHex(0xff6b7280);
[[gnu::used]] static constexpr Color GRAY600 = Color::fromHex(0xff4b5563);
[[gnu::used]] static constexpr Color GRAY700 = Color::fromHex(0xff374151);
[[gnu::used]] static constexpr Color GRAY800 = Color::fromHex(0xff1f2937);
[[gnu::used]] static constexpr Color GRAY900 = Color::fromHex(0xff111827);

[[gnu::used]] static constexpr Color ZINC50 = Color::fromHex(0xfffafafa);
[[gnu::used]] static constexpr Color ZINC100 = Color::fromHex(0xfff4f4f5);
[[gnu::used]] static constexpr Color ZINC200 = Color::fromHex(0xffe4e4e7);
[[gnu::used]] static constexpr Color ZINC300 = Color::fromHex(0xffd4d4d8);
[[gnu::used]] static constexpr Color ZINC400 = Color::fromHex(0xffa1a1aa);
[[gnu::used]] static constexpr Color ZINC500 = Color::fromHex(0xff71717a);
[[gnu::used]] static constexpr Color ZINC600 = Color::fromHex(0xff52525b);
[[gnu::used]] static constexpr Color ZINC700 = Color::fromHex(0xff3f3f46);
[[gnu::used]] static constexpr Color ZINC800 = Color::fromHex(0xff27272a);
[[gnu::used]] static constexpr Color ZINC900 = Color::fromHex(0xff18181b);

[[gnu::used]] static constexpr Color NEUTRAL50 = Color::fromHex(0xfffafafa);
[[gnu::used]] static constexpr Color NEUTRAL100 = Color::fromHex(0xfff5f5f5);
[[gnu::used]] static constexpr Color NEUTRAL200 = Color::fromHex(0xffe5e5e5);
[[gnu::used]] static constexpr Color NEUTRAL300 = Color::fromHex(0xffd4d4d4);
[[gnu::used]] static constexpr Color NEUTRAL400 = Color::fromHex(0xffa3a3a3);
[[gnu::used]] static constexpr Color NEUTRAL500 = Color::fromHex(0xff737373);
[[gnu::used]] static constexpr Color NEUTRAL600 = Color::fromHex(0xff525252);
[[gnu::used]] static constexpr Color NEUTRAL700 = Color::fromHex(0xff404040);
[[gnu::used]] static constexpr Color NEUTRAL800 = Color::fromHex(0xff262626);
[[gnu::used]] static constexpr Color NEUTRAL900 = Color::fromHex(0xff171717);

[[gnu::used]] static constexpr Color STONE50 = Color::fromHex(0xfffafaf9);
[[gnu::used]] static constexpr Color STONE100 = Color::fromHex(0xfff5f5f4);
[[gnu::used]] static constexpr Color STONE200 = Color::fromHex(0xffe7e5e4);
[[gnu::used]] static constexpr Color STONE300 = Color::fromHex(0xffd6d3d1);
[[gnu::used]] static constexpr Color STONE400 = Color::fromHex(0xffa8a29e);
[[gnu::used]] static constexpr Color STONE500 = Color::fromHex(0xff78716c);
[[gnu::used]] static constexpr Color STONE600 = Color::fromHex(0xff57534e);
[[gnu::used]] static constexpr Color STONE700 = Color::fromHex(0xff44403c);
[[gnu::used]] static constexpr Color STONE800 = Color::fromHex(0xff292524);
[[gnu::used]] static constexpr Color STONE900 = Color::fromHex(0xff1c1917);

[[gnu::used]] static constexpr Color RED50 = Color::fromHex(0xfffef2f2);
[[gnu::used]] static constexpr Color RED100 = Color::fromHex(0xfffee2e2);
[[gnu::used]] static constexpr Color RED200 = Color::fromHex(0xfffecaca);
[[gnu::used]] static constexpr Color RED300 = Color::fromHex(0xfffca5a5);
[[gnu::used]] static constexpr Color RED400 = Color::fromHex(0xfff87171);
[[gnu::used]] static constexpr Color RED500 = Color::fromHex(0xffef4444);
[[gnu::used]] static constexpr Color RED600 = Color::fromHex(0xffdc2626);
[[gnu::used]] static constexpr Color RED700 = Color::fromHex(0xffb91c1c);
[[gnu::used]] static constexpr Color RED800 = Color::fromHex(0xff991b1b);
[[gnu::used]] static constexpr Color RED900 = Color::fromHex(0xff7f1d1d);

[[gnu::used]] static constexpr Color ORANGE50 = Color::fromHex(0xfffff7ed);
[[gnu::used]] static constexpr Color ORANGE100 = Color::fromHex(0xffffedd5);
[[gnu::used]] static constexpr Color ORANGE200 = Color::fromHex(0xfffed7aa);
[[gnu::used]] static constexpr Color ORANGE300 = Color::fromHex(0xfffdba74);
[[gnu::used]] static constexpr Color ORANGE400 = Color::fromHex(0xfffb923c);
[[gnu::used]] static constexpr Color ORANGE500 = Color::fromHex(0xfff97316);
[[gnu::used]] static constexpr Color ORANGE600 = Color::fromHex(0xffea580c);
[[gnu::used]] static constexpr Color ORANGE700 = Color::fromHex(0xffc2410c);
[[gnu::used]] static constexpr Color ORANGE800 = Color::fromHex(0xff9a3412);
[[gnu::used]] static constexpr Color ORANGE900 = Color::fromHex(0xff7c2d12);

[[gnu::used]] static constexpr Color AMBER50 = Color::fromHex(0xfffffbeb);
[[gnu::used]] static constexpr Color AMBER100 = Color::fromHex(0xfffef3c7);
[[gnu::used]] static constexpr Color AMBER200 = Color::fromHex(0xfffde68a);
[[gnu::used]] static constexpr Color AMBER300 = Color::fromHex(0xfffcd34d);
[[gnu::used]] static constexpr Color AMBER400 = Color::fromHex(0xfffbbf24);
[[gnu::used]] static constexpr Color AMBER500 = Color::fromHex(0xfff59e0b);
[[gnu::used]] static constexpr Color AMBER600 = Color::fromHex(0xffd97706);
[[gnu::used]] static constexpr Color AMBER700 = Color::fromHex(0xffb45309);
[[gnu::used]] static constexpr Color AMBER800 = Color::fromHex(0xff92400e);
[[gnu::used]] static constexpr Color AMBER900 = Color::fromHex(0xff78350f);

[[gnu::used]] static constexpr Color YELLOW50 = Color::fromHex(0xfffefce8);
[[gnu::used]] static constexpr Color YELLOW100 = Color::fromHex(0xfffef9c3);
[[gnu::used]] static constexpr Color YELLOW200 = Color::fromHex(0xfffef08a);
[[gnu::used]] static constexpr Color YELLOW300 = Color::fromHex(0xfffde047);
[[gnu::used]] static constexpr Color YELLOW400 = Color::fromHex(0xfffacc15);
[[gnu::used]] static constexpr Color YELLOW500 = Color::fromHex(0xffeab308);
[[gnu::used]] static constexpr Color YELLOW600 = Color::fromHex(0xffca8a04);
[[gnu::used]] static constexpr Color YELLOW700 = Color::fromHex(0xffa16207);
[[gnu::used]] static constexpr Color YELLOW800 = Color::fromHex(0xff854d0e);
[[gnu::used]] static constexpr Color YELLOW900 = Color::fromHex(0xff713f12);

[[gnu::used]] static constexpr Color LIME50 = Color::fromHex(0xfff7fee7);
[[gnu::used]] static constexpr Color LIME100 = Color::fromHex(0xffecfccb);
[[gnu::used]] static constexpr Color LIME200 = Color::fromHex(0xffd9f99d);
[[gnu::used]] static constexpr Color LIME300 = Color::fromHex(0xffbef264);
[[gnu::used]] static constexpr Color LIME400 = Color::fromHex(0xffa3e635);
[[gnu::used]] static constexpr Color LIME500 = Color::fromHex(0xff84cc16);
[[gnu::used]] static constexpr Color LIME600 = Color::fromHex(0xff65a30d);
[[gnu::used]] static constexpr Color LIME700 = Color::fromHex(0xff4d7c0f);
[[gnu::used]] static constexpr Color LIME800 = Color::fromHex(0xff3f6212);
[[gnu::used]] static constexpr Color LIME900 = Color::fromHex(0xff365314);

[[gnu::used]] static constexpr Color GREEN50 = Color::fromHex(0xfff0fdf4);
[[gnu::used]] static constexpr Color GREEN100 = Color::fromHex(0xffdcfce7);
[[gnu::used]] static constexpr Color GREEN200 = Color::fromHex(0xffbbf7d0);
[[gnu::used]] static constexpr Color GREEN300 = Color::fromHex(0xff86efac);
[[gnu::used]] static constexpr Color GREEN400 = Color::fromHex(0xff4ade80);
[[gnu::used]] static constexpr Color GREEN500 = Color::fromHex(0xff22c55e);
[[gnu::used]] static constexpr Color GREEN600 = Color::fromHex(0xff16a34a);
[[gnu::used]] static constexpr Color GREEN700 = Color::fromHex(0xff15803d);
[[gnu::used]] static constexpr Color GREEN800 = Color::fromHex(0xff166534);
[[gnu::used]] static constexpr Color GREEN900 = Color::fromHex(0xff14532d);

[[gnu::used]] static constexpr Color EMERALD50 = Color::fromHex(0xffecfdf5);
[[gnu::used]] static constexpr Color EMERALD100 = Color::fromHex(0xffd1fae5);
[[gnu::used]] static constexpr Color EMERALD200 = Color::fromHex(0xffa7f3d0);
[[gnu::used]] static constexpr Color EMERALD300 = Color::fromHex(0xff6ee7b7);
[[gnu::used]] static constexpr Color EMERALD400 = Color::fromHex(0xff34d399);
[[gnu::used]] static constexpr Color EMERALD500 = Color::fromHex(0xff10b981);
[[gnu::used]] static constexpr Color EMERALD600 = Color::fromHex(0xff059669);
[[gnu::used]] static constexpr Color EMERALD700 = Color::fromHex(0xff047857);
[[gnu::used]] static constexpr Color EMERALD800 = Color::fromHex(0xff065f46);
[[gnu::used]] static constexpr Color EMERALD900 = Color::fromHex(0xff064e3b);

[[gnu::used]] static constexpr Color TEAL50 = Color::fromHex(0xfff0fdfa);
[[gnu::used]] static constexpr Color TEAL100 = Color::fromHex(0xffccfbf1);
[[gnu::used]] static constexpr Color TEAL200 = Color::fromHex(0xff99f6e4);
[[gnu::used]] static constexpr Color TEAL300 = Color::fromHex(0xff5eead4);
[[gnu::used]] static constexpr Color TEAL400 = Color::fromHex(0xff2dd4bf);
[[gnu::used]] static constexpr Color TEAL500 = Color::fromHex(0xff14b8a6);
[[gnu::used]] static constexpr Color TEAL600 = Color::fromHex(0xff0d9488);
[[gnu::used]] static constexpr Color TEAL700 = Color::fromHex(0xff0f766e);
[[gnu::used]] static constexpr Color TEAL800 = Color::fromHex(0xff115e59);
[[gnu::used]] static constexpr Color TEAL900 = Color::fromHex(0xff134e4a);

[[gnu::used]] static constexpr Color CYAN50 = Color::fromHex(0xffecfeff);
[[gnu::used]] static constexpr Color CYAN100 = Color::fromHex(0xffcffafe);
[[gnu::used]] static constexpr Color CYAN200 = Color::fromHex(0xffa5f3fc);
[[gnu::used]] static constexpr Color CYAN300 = Color::fromHex(0xff67e8f9);
[[gnu::used]] static constexpr Color CYAN400 = Color::fromHex(0xff22d3ee);
[[gnu::used]] static constexpr Color CYAN500 = Color::fromHex(0xff06b6d4);
[[gnu::used]] static constexpr Color CYAN600 = Color::fromHex(0xff0891b2);
[[gnu::used]] static constexpr Color CYAN700 = Color::fromHex(0xff0e7490);
[[gnu::used]] static constexpr Color CYAN800 = Color::fromHex(0xff155e75);
[[gnu::used]] static constexpr Color CYAN900 = Color::fromHex(0xff164e63);

[[gnu::used]] static constexpr Color SKY50 = Color::fromHex(0xfff0f9ff);
[[gnu::used]] static constexpr Color SKY100 = Color::fromHex(0xffe0f2fe);
[[gnu::used]] static constexpr Color SKY200 = Color::fromHex(0xffbae6fd);
[[gnu::used]] static constexpr Color SKY300 = Color::fromHex(0xff7dd3fc);
[[gnu::used]] static constexpr Color SKY400 = Color::fromHex(0xff38bdf8);
[[gnu::used]] static constexpr Color SKY500 = Color::fromHex(0xff0ea5e9);
[[gnu::used]] static constexpr Color SKY600 = Color::fromHex(0xff0284c7);
[[gnu::used]] static constexpr Color SKY700 = Color::fromHex(0xff0369a1);
[[gnu::used]] static constexpr Color SKY800 = Color::fromHex(0xff075985);
[[gnu::used]] static constexpr Color SKY900 = Color::fromHex(0xff0c4a6e);

[[gnu::used]] static constexpr Color BLUE50 = Color::fromHex(0xffeff6ff);
[[gnu::used]] static constexpr Color BLUE100 = Color::fromHex(0xffdbeafe);
[[gnu::used]] static constexpr Color BLUE200 = Color::fromHex(0xffbfdbfe);
[[gnu::used]] static constexpr Color BLUE300 = Color::fromHex(0xff93c5fd);
[[gnu::used]] static constexpr Color BLUE400 = Color::fromHex(0xff60a5fa);
[[gnu::used]] static constexpr Color BLUE500 = Color::fromHex(0xff3b82f6);
[[gnu::used]] static constexpr Color BLUE600 = Color::fromHex(0xff2563eb);
[[gnu::used]] static constexpr Color BLUE700 = Color::fromHex(0xff1d4ed8);
[[gnu::used]] static constexpr Color BLUE800 = Color::fromHex(0xff1e40af);
[[gnu::used]] static constexpr Color BLUE900 = Color::fromHex(0xff1e3a8a);

[[gnu::used]] static constexpr Color INDIGO50 = Color::fromHex(0xffeef2ff);
[[gnu::used]] static constexpr Color INDIGO100 = Color::fromHex(0xffe0e7ff);
[[gnu::used]] static constexpr Color INDIGO200 = Color::fromHex(0xffc7d2fe);
[[gnu::used]] static constexpr Color INDIGO300 = Color::fromHex(0xffa5b4fc);
[[gnu::used]] static constexpr Color INDIGO400 = Color::fromHex(0xff818cf8);
[[gnu::used]] static constexpr Color INDIGO500 = Color::fromHex(0xff6366f1);
[[gnu::used]] static constexpr Color INDIGO600 = Color::fromHex(0xff4f46e5);
[[gnu::used]] static constexpr Color INDIGO700 = Color::fromHex(0xff4338ca);
[[gnu::used]] static constexpr Color INDIGO800 = Color::fromHex(0xff3730a3);
[[gnu::used]] static constexpr Color INDIGO900 = Color::fromHex(0xff312e81);

[[gnu::used]] static constexpr Color VIOLET50 = Color::fromHex(0xfff5f3ff);
[[gnu::used]] static constexpr Color VIOLET100 = Color::fromHex(0xffede9fe);
[[gnu::used]] static constexpr Color VIOLET200 = Color::fromHex(0xffddd6fe);
[[gnu::used]] static constexpr Color VIOLET300 = Color::fromHex(0xffc4b5fd);
[[gnu::used]] static constexpr Color VIOLET400 = Color::fromHex(0xffa78bfa);
[[gnu::used]] static constexpr Color VIOLET500 = Color::fromHex(0xff8b5cf6);
[[gnu::used]] static constexpr Color VIOLET600 = Color::fromHex(0xff7c3aed);
[[gnu::used]] static constexpr Color VIOLET700 = Color::fromHex(0xff6d28d9);
[[gnu::used]] static constexpr Color VIOLET800 = Color::fromHex(0xff5b21b6);
[[gnu::used]] static constexpr Color VIOLET900 = Color::fromHex(0xff4c1d95);

[[gnu::used]] static constexpr Color PURPLE50 = Color::fromHex(0xfffaf5ff);
[[gnu::used]] static constexpr Color PURPLE100 = Color::fromHex(0xfff3e8ff);
[[gnu::used]] static constexpr Color PURPLE200 = Color::fromHex(0xffe9d5ff);
[[gnu::used]] static constexpr Color PURPLE300 = Color::fromHex(0xffd8b4fe);
[[gnu::used]] static constexpr Color PURPLE400 = Color::fromHex(0xffc084fc);
[[gnu::used]] static constexpr Color PURPLE500 = Color::fromHex(0xffa855f7);
[[gnu::used]] static constexpr Color PURPLE600 = Color::fromHex(0xff9333ea);
[[gnu::used]] static constexpr Color PURPLE700 = Color::fromHex(0xff7e22ce);
[[gnu::used]] static constexpr Color PURPLE800 = Color::fromHex(0xff6b21a8);
[[gnu::used]] static constexpr Color PURPLE900 = Color::fromHex(0xff581c87);

[[gnu::used]] static constexpr Color FUCHSIA50 = Color::fromHex(0xfffdf4ff);
[[gnu::used]] static constexpr Color FUCHSIA100 = Color::fromHex(0xfffae8ff);
[[gnu::used]] static constexpr Color FUCHSIA200 = Color::fromHex(0xfff5d0fe);
[[gnu::used]] static constexpr Color FUCHSIA300 = Color::fromHex(0xfff0abfc);
[[gnu::used]] static constexpr Color FUCHSIA400 = Color::fromHex(0xffe879f9);
[[gnu::used]] static constexpr Color FUCHSIA500 = Color::fromHex(0xffd946ef);
[[gnu::used]] static constexpr Color FUCHSIA600 = Color::fromHex(0xffc026d3);
[[gnu::used]] static constexpr Color FUCHSIA700 = Color::fromHex(0xffa21caf);
[[gnu::used]] static constexpr Color FUCHSIA800 = Color::fromHex(0xff86198f);
[[gnu::used]] static constexpr Color FUCHSIA900 = Color::fromHex(0xff701a75);

[[gnu::used]] static constexpr Color PINK50 = Color::fromHex(0xfffdf2f8);
[[gnu::used]] static constexpr Color PINK100 = Color::fromHex(0xfffce7f3);
[[gnu::used]] static constexpr Color PINK200 = Color::fromHex(0xfffbcfe8);
[[gnu::used]] static constexpr Color PINK300 = Color::fromHex(0xfff9a8d4);
[[gnu::used]] static constexpr Color PINK400 = Color::fromHex(0xfff472b6);
[[gnu::used]] static constexpr Color PINK500 = Color::fromHex(0xffec4899);
[[gnu::used]] static constexpr Color PINK600 = Color::fromHex(0xffdb2777);
[[gnu::used]] static constexpr Color PINK700 = Color::fromHex(0xffbe185d);
[[gnu::used]] static constexpr Color PINK800 = Color::fromHex(0xff9d174d);
[[gnu::used]] static constexpr Color PINK900 = Color::fromHex(0xff831843);

[[gnu::used]] static constexpr Color ROSE50 = Color::fromHex(0xfffff1f2);
[[gnu::used]] static constexpr Color ROSE100 = Color::fromHex(0xffffe4e6);
[[gnu::used]] static constexpr Color ROSE200 = Color::fromHex(0xfffecdd3);
[[gnu::used]] static constexpr Color ROSE300 = Color::fromHex(0xfffda4af);
[[gnu::used]] static constexpr Color ROSE400 = Color::fromHex(0xfffb7185);
[[gnu::used]] static constexpr Color ROSE500 = Color::fromHex(0xfff43f5e);
[[gnu::used]] static constexpr Color ROSE600 = Color::fromHex(0xffe11d48);
[[gnu::used]] static constexpr Color ROSE700 = Color::fromHex(0xffbe123c);
[[gnu::used]] static constexpr Color ROSE800 = Color::fromHex(0xff9f1239);
[[gnu::used]] static constexpr Color ROSE900 = Color::fromHex(0xff881337);

} // namespace Karm::Gfx
