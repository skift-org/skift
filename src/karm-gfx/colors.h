#pragma once

#include <karm-gfx/color.h>

namespace Karm::Gfx {

namespace Colors {

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
[[gnu::used]] static constexpr Color SLATE50 = Color::fromHex(0xf8fafc);
[[gnu::used]] static constexpr Color SLATE100 = Color::fromHex(0xf1f5f9);
[[gnu::used]] static constexpr Color SLATE200 = Color::fromHex(0xe2e8f0);
[[gnu::used]] static constexpr Color SLATE300 = Color::fromHex(0xcbd5e1);
[[gnu::used]] static constexpr Color SLATE400 = Color::fromHex(0x94a3b8);
[[gnu::used]] static constexpr Color SLATE500 = Color::fromHex(0x64748b);
[[gnu::used]] static constexpr Color SLATE600 = Color::fromHex(0x475569);
[[gnu::used]] static constexpr Color SLATE700 = Color::fromHex(0x334155);
[[gnu::used]] static constexpr Color SLATE800 = Color::fromHex(0x1e293b);
[[gnu::used]] static constexpr Color SLATE900 = Color::fromHex(0x0f172a);

[[gnu::used]] static constexpr Color GRAY50 = Color::fromHex(0xf9fafb);
[[gnu::used]] static constexpr Color GRAY100 = Color::fromHex(0xf3f4f6);
[[gnu::used]] static constexpr Color GRAY200 = Color::fromHex(0xe5e7eb);
[[gnu::used]] static constexpr Color GRAY300 = Color::fromHex(0xd1d5db);
[[gnu::used]] static constexpr Color GRAY400 = Color::fromHex(0x9ca3af);
[[gnu::used]] static constexpr Color GRAY500 = Color::fromHex(0x6b7280);
[[gnu::used]] static constexpr Color GRAY600 = Color::fromHex(0x4b5563);
[[gnu::used]] static constexpr Color GRAY700 = Color::fromHex(0x374151);
[[gnu::used]] static constexpr Color GRAY800 = Color::fromHex(0x1f2937);
[[gnu::used]] static constexpr Color GRAY900 = Color::fromHex(0x111827);

[[gnu::used]] static constexpr Color ZINC50 = Color::fromHex(0xfafafa);
[[gnu::used]] static constexpr Color ZINC100 = Color::fromHex(0xf4f4f5);
[[gnu::used]] static constexpr Color ZINC200 = Color::fromHex(0xe4e4e7);
[[gnu::used]] static constexpr Color ZINC300 = Color::fromHex(0xd4d4d8);
[[gnu::used]] static constexpr Color ZINC400 = Color::fromHex(0xa1a1aa);
[[gnu::used]] static constexpr Color ZINC500 = Color::fromHex(0x71717a);
[[gnu::used]] static constexpr Color ZINC600 = Color::fromHex(0x52525b);
[[gnu::used]] static constexpr Color ZINC700 = Color::fromHex(0x3f3f46);
[[gnu::used]] static constexpr Color ZINC800 = Color::fromHex(0x27272a);
[[gnu::used]] static constexpr Color ZINC900 = Color::fromHex(0x18181b);

[[gnu::used]] static constexpr Color NEUTRAL50 = Color::fromHex(0xfafafa);
[[gnu::used]] static constexpr Color NEUTRAL100 = Color::fromHex(0xf5f5f5);
[[gnu::used]] static constexpr Color NEUTRAL200 = Color::fromHex(0xe5e5e5);
[[gnu::used]] static constexpr Color NEUTRAL300 = Color::fromHex(0xd4d4d4);
[[gnu::used]] static constexpr Color NEUTRAL400 = Color::fromHex(0xa3a3a3);
[[gnu::used]] static constexpr Color NEUTRAL500 = Color::fromHex(0x737373);
[[gnu::used]] static constexpr Color NEUTRAL600 = Color::fromHex(0x525252);
[[gnu::used]] static constexpr Color NEUTRAL700 = Color::fromHex(0x404040);
[[gnu::used]] static constexpr Color NEUTRAL800 = Color::fromHex(0x262626);
[[gnu::used]] static constexpr Color NEUTRAL900 = Color::fromHex(0x171717);

[[gnu::used]] static constexpr Color STONE50 = Color::fromHex(0xfafaf9);
[[gnu::used]] static constexpr Color STONE100 = Color::fromHex(0xf5f5f4);
[[gnu::used]] static constexpr Color STONE200 = Color::fromHex(0xe7e5e4);
[[gnu::used]] static constexpr Color STONE300 = Color::fromHex(0xd6d3d1);
[[gnu::used]] static constexpr Color STONE400 = Color::fromHex(0xa8a29e);
[[gnu::used]] static constexpr Color STONE500 = Color::fromHex(0x78716c);
[[gnu::used]] static constexpr Color STONE600 = Color::fromHex(0x57534e);
[[gnu::used]] static constexpr Color STONE700 = Color::fromHex(0x44403c);
[[gnu::used]] static constexpr Color STONE800 = Color::fromHex(0x292524);
[[gnu::used]] static constexpr Color STONE900 = Color::fromHex(0x1c1917);

[[gnu::used]] static constexpr Color RED50 = Color::fromHex(0xfef2f2);
[[gnu::used]] static constexpr Color RED100 = Color::fromHex(0xfee2e2);
[[gnu::used]] static constexpr Color RED200 = Color::fromHex(0xfecaca);
[[gnu::used]] static constexpr Color RED300 = Color::fromHex(0xfca5a5);
[[gnu::used]] static constexpr Color RED400 = Color::fromHex(0xf87171);
[[gnu::used]] static constexpr Color RED500 = Color::fromHex(0xef4444);
[[gnu::used]] static constexpr Color RED600 = Color::fromHex(0xdc2626);
[[gnu::used]] static constexpr Color RED700 = Color::fromHex(0xb91c1c);
[[gnu::used]] static constexpr Color RED800 = Color::fromHex(0x991b1b);
[[gnu::used]] static constexpr Color RED900 = Color::fromHex(0x7f1d1d);

[[gnu::used]] static constexpr Color ORANGE50 = Color::fromHex(0xfff7ed);
[[gnu::used]] static constexpr Color ORANGE100 = Color::fromHex(0xffedd5);
[[gnu::used]] static constexpr Color ORANGE200 = Color::fromHex(0xfed7aa);
[[gnu::used]] static constexpr Color ORANGE300 = Color::fromHex(0xfdba74);
[[gnu::used]] static constexpr Color ORANGE400 = Color::fromHex(0xfb923c);
[[gnu::used]] static constexpr Color ORANGE500 = Color::fromHex(0xf97316);
[[gnu::used]] static constexpr Color ORANGE600 = Color::fromHex(0xea580c);
[[gnu::used]] static constexpr Color ORANGE700 = Color::fromHex(0xc2410c);
[[gnu::used]] static constexpr Color ORANGE800 = Color::fromHex(0x9a3412);
[[gnu::used]] static constexpr Color ORANGE900 = Color::fromHex(0x7c2d12);

[[gnu::used]] static constexpr Color AMBER50 = Color::fromHex(0xfffbeb);
[[gnu::used]] static constexpr Color AMBER100 = Color::fromHex(0xfef3c7);
[[gnu::used]] static constexpr Color AMBER200 = Color::fromHex(0xfde68a);
[[gnu::used]] static constexpr Color AMBER300 = Color::fromHex(0xfcd34d);
[[gnu::used]] static constexpr Color AMBER400 = Color::fromHex(0xfbbf24);
[[gnu::used]] static constexpr Color AMBER500 = Color::fromHex(0xf59e0b);
[[gnu::used]] static constexpr Color AMBER600 = Color::fromHex(0xd97706);
[[gnu::used]] static constexpr Color AMBER700 = Color::fromHex(0xb45309);
[[gnu::used]] static constexpr Color AMBER800 = Color::fromHex(0x92400e);
[[gnu::used]] static constexpr Color AMBER900 = Color::fromHex(0x78350f);

[[gnu::used]] static constexpr Color YELLOW50 = Color::fromHex(0xfefce8);
[[gnu::used]] static constexpr Color YELLOW100 = Color::fromHex(0xfef9c3);
[[gnu::used]] static constexpr Color YELLOW200 = Color::fromHex(0xfef08a);
[[gnu::used]] static constexpr Color YELLOW300 = Color::fromHex(0xfde047);
[[gnu::used]] static constexpr Color YELLOW400 = Color::fromHex(0xfacc15);
[[gnu::used]] static constexpr Color YELLOW500 = Color::fromHex(0xeab308);
[[gnu::used]] static constexpr Color YELLOW600 = Color::fromHex(0xca8a04);
[[gnu::used]] static constexpr Color YELLOW700 = Color::fromHex(0xa16207);
[[gnu::used]] static constexpr Color YELLOW800 = Color::fromHex(0x854d0e);
[[gnu::used]] static constexpr Color YELLOW900 = Color::fromHex(0x713f12);

[[gnu::used]] static constexpr Color LIME50 = Color::fromHex(0xf7fee7);
[[gnu::used]] static constexpr Color LIME100 = Color::fromHex(0xecfccb);
[[gnu::used]] static constexpr Color LIME200 = Color::fromHex(0xd9f99d);
[[gnu::used]] static constexpr Color LIME300 = Color::fromHex(0xbef264);
[[gnu::used]] static constexpr Color LIME400 = Color::fromHex(0xa3e635);
[[gnu::used]] static constexpr Color LIME500 = Color::fromHex(0x84cc16);
[[gnu::used]] static constexpr Color LIME600 = Color::fromHex(0x65a30d);
[[gnu::used]] static constexpr Color LIME700 = Color::fromHex(0x4d7c0f);
[[gnu::used]] static constexpr Color LIME800 = Color::fromHex(0x3f6212);
[[gnu::used]] static constexpr Color LIME900 = Color::fromHex(0x365314);

[[gnu::used]] static constexpr Color GREEN50 = Color::fromHex(0xf0fdf4);
[[gnu::used]] static constexpr Color GREEN100 = Color::fromHex(0xdcfce7);
[[gnu::used]] static constexpr Color GREEN200 = Color::fromHex(0xbbf7d0);
[[gnu::used]] static constexpr Color GREEN300 = Color::fromHex(0x86efac);
[[gnu::used]] static constexpr Color GREEN400 = Color::fromHex(0x4ade80);
[[gnu::used]] static constexpr Color GREEN500 = Color::fromHex(0x22c55e);
[[gnu::used]] static constexpr Color GREEN600 = Color::fromHex(0x16a34a);
[[gnu::used]] static constexpr Color GREEN700 = Color::fromHex(0x15803d);
[[gnu::used]] static constexpr Color GREEN800 = Color::fromHex(0x166534);
[[gnu::used]] static constexpr Color GREEN900 = Color::fromHex(0x14532d);

[[gnu::used]] static constexpr Color EMERALD50 = Color::fromHex(0xecfdf5);
[[gnu::used]] static constexpr Color EMERALD100 = Color::fromHex(0xd1fae5);
[[gnu::used]] static constexpr Color EMERALD200 = Color::fromHex(0xa7f3d0);
[[gnu::used]] static constexpr Color EMERALD300 = Color::fromHex(0x6ee7b7);
[[gnu::used]] static constexpr Color EMERALD400 = Color::fromHex(0x34d399);
[[gnu::used]] static constexpr Color EMERALD500 = Color::fromHex(0x10b981);
[[gnu::used]] static constexpr Color EMERALD600 = Color::fromHex(0x059669);
[[gnu::used]] static constexpr Color EMERALD700 = Color::fromHex(0x047857);
[[gnu::used]] static constexpr Color EMERALD800 = Color::fromHex(0x065f46);
[[gnu::used]] static constexpr Color EMERALD900 = Color::fromHex(0x064e3b);

[[gnu::used]] static constexpr Color TEAL50 = Color::fromHex(0xf0fdfa);
[[gnu::used]] static constexpr Color TEAL100 = Color::fromHex(0xccfbf1);
[[gnu::used]] static constexpr Color TEAL200 = Color::fromHex(0x99f6e4);
[[gnu::used]] static constexpr Color TEAL300 = Color::fromHex(0x5eead4);
[[gnu::used]] static constexpr Color TEAL400 = Color::fromHex(0x2dd4bf);
[[gnu::used]] static constexpr Color TEAL500 = Color::fromHex(0x14b8a6);
[[gnu::used]] static constexpr Color TEAL600 = Color::fromHex(0x0d9488);
[[gnu::used]] static constexpr Color TEAL700 = Color::fromHex(0x0f766e);
[[gnu::used]] static constexpr Color TEAL800 = Color::fromHex(0x115e59);
[[gnu::used]] static constexpr Color TEAL900 = Color::fromHex(0x134e4a);

[[gnu::used]] static constexpr Color CYAN50 = Color::fromHex(0xecfeff);
[[gnu::used]] static constexpr Color CYAN100 = Color::fromHex(0xcffafe);
[[gnu::used]] static constexpr Color CYAN200 = Color::fromHex(0xa5f3fc);
[[gnu::used]] static constexpr Color CYAN300 = Color::fromHex(0x67e8f9);
[[gnu::used]] static constexpr Color CYAN400 = Color::fromHex(0x22d3ee);
[[gnu::used]] static constexpr Color CYAN500 = Color::fromHex(0x06b6d4);
[[gnu::used]] static constexpr Color CYAN600 = Color::fromHex(0x0891b2);
[[gnu::used]] static constexpr Color CYAN700 = Color::fromHex(0x0e7490);
[[gnu::used]] static constexpr Color CYAN800 = Color::fromHex(0x155e75);
[[gnu::used]] static constexpr Color CYAN900 = Color::fromHex(0x164e63);

[[gnu::used]] static constexpr Color SKY50 = Color::fromHex(0xf0f9ff);
[[gnu::used]] static constexpr Color SKY100 = Color::fromHex(0xe0f2fe);
[[gnu::used]] static constexpr Color SKY200 = Color::fromHex(0xbae6fd);
[[gnu::used]] static constexpr Color SKY300 = Color::fromHex(0x7dd3fc);
[[gnu::used]] static constexpr Color SKY400 = Color::fromHex(0x38bdf8);
[[gnu::used]] static constexpr Color SKY500 = Color::fromHex(0x0ea5e9);
[[gnu::used]] static constexpr Color SKY600 = Color::fromHex(0x0284c7);
[[gnu::used]] static constexpr Color SKY700 = Color::fromHex(0x0369a1);
[[gnu::used]] static constexpr Color SKY800 = Color::fromHex(0x075985);
[[gnu::used]] static constexpr Color SKY900 = Color::fromHex(0x0c4a6e);

[[gnu::used]] static constexpr Color BLUE50 = Color::fromHex(0xeff6ff);
[[gnu::used]] static constexpr Color BLUE100 = Color::fromHex(0xdbeafe);
[[gnu::used]] static constexpr Color BLUE200 = Color::fromHex(0xbfdbfe);
[[gnu::used]] static constexpr Color BLUE300 = Color::fromHex(0x93c5fd);
[[gnu::used]] static constexpr Color BLUE400 = Color::fromHex(0x60a5fa);
[[gnu::used]] static constexpr Color BLUE500 = Color::fromHex(0x3b82f6);
[[gnu::used]] static constexpr Color BLUE600 = Color::fromHex(0x2563eb);
[[gnu::used]] static constexpr Color BLUE700 = Color::fromHex(0x1d4ed8);
[[gnu::used]] static constexpr Color BLUE800 = Color::fromHex(0x1e40af);
[[gnu::used]] static constexpr Color BLUE900 = Color::fromHex(0x1e3a8a);

[[gnu::used]] static constexpr Color INDIGO50 = Color::fromHex(0xeef2ff);
[[gnu::used]] static constexpr Color INDIGO100 = Color::fromHex(0xe0e7ff);
[[gnu::used]] static constexpr Color INDIGO200 = Color::fromHex(0xc7d2fe);
[[gnu::used]] static constexpr Color INDIGO300 = Color::fromHex(0xa5b4fc);
[[gnu::used]] static constexpr Color INDIGO400 = Color::fromHex(0x818cf8);
[[gnu::used]] static constexpr Color INDIGO500 = Color::fromHex(0x6366f1);
[[gnu::used]] static constexpr Color INDIGO600 = Color::fromHex(0x4f46e5);
[[gnu::used]] static constexpr Color INDIGO700 = Color::fromHex(0x4338ca);
[[gnu::used]] static constexpr Color INDIGO800 = Color::fromHex(0x3730a3);
[[gnu::used]] static constexpr Color INDIGO900 = Color::fromHex(0x312e81);

[[gnu::used]] static constexpr Color VIOLET50 = Color::fromHex(0xf5f3ff);
[[gnu::used]] static constexpr Color VIOLET100 = Color::fromHex(0xede9fe);
[[gnu::used]] static constexpr Color VIOLET200 = Color::fromHex(0xddd6fe);
[[gnu::used]] static constexpr Color VIOLET300 = Color::fromHex(0xc4b5fd);
[[gnu::used]] static constexpr Color VIOLET400 = Color::fromHex(0xa78bfa);
[[gnu::used]] static constexpr Color VIOLET500 = Color::fromHex(0x8b5cf6);
[[gnu::used]] static constexpr Color VIOLET600 = Color::fromHex(0x7c3aed);
[[gnu::used]] static constexpr Color VIOLET700 = Color::fromHex(0x6d28d9);
[[gnu::used]] static constexpr Color VIOLET800 = Color::fromHex(0x5b21b6);
[[gnu::used]] static constexpr Color VIOLET900 = Color::fromHex(0x4c1d95);

[[gnu::used]] static constexpr Color PURPLE50 = Color::fromHex(0xfaf5ff);
[[gnu::used]] static constexpr Color PURPLE100 = Color::fromHex(0xf3e8ff);
[[gnu::used]] static constexpr Color PURPLE200 = Color::fromHex(0xe9d5ff);
[[gnu::used]] static constexpr Color PURPLE300 = Color::fromHex(0xd8b4fe);
[[gnu::used]] static constexpr Color PURPLE400 = Color::fromHex(0xc084fc);
[[gnu::used]] static constexpr Color PURPLE500 = Color::fromHex(0xa855f7);
[[gnu::used]] static constexpr Color PURPLE600 = Color::fromHex(0x9333ea);
[[gnu::used]] static constexpr Color PURPLE700 = Color::fromHex(0x7e22ce);
[[gnu::used]] static constexpr Color PURPLE800 = Color::fromHex(0x6b21a8);
[[gnu::used]] static constexpr Color PURPLE900 = Color::fromHex(0x581c87);

[[gnu::used]] static constexpr Color FUCHSIA50 = Color::fromHex(0xfdf4ff);
[[gnu::used]] static constexpr Color FUCHSIA100 = Color::fromHex(0xfae8ff);
[[gnu::used]] static constexpr Color FUCHSIA200 = Color::fromHex(0xf5d0fe);
[[gnu::used]] static constexpr Color FUCHSIA300 = Color::fromHex(0xf0abfc);
[[gnu::used]] static constexpr Color FUCHSIA400 = Color::fromHex(0xe879f9);
[[gnu::used]] static constexpr Color FUCHSIA500 = Color::fromHex(0xd946ef);
[[gnu::used]] static constexpr Color FUCHSIA600 = Color::fromHex(0xc026d3);
[[gnu::used]] static constexpr Color FUCHSIA700 = Color::fromHex(0xa21caf);
[[gnu::used]] static constexpr Color FUCHSIA800 = Color::fromHex(0x86198f);
[[gnu::used]] static constexpr Color FUCHSIA900 = Color::fromHex(0x701a75);

[[gnu::used]] static constexpr Color PINK50 = Color::fromHex(0xfdf2f8);
[[gnu::used]] static constexpr Color PINK100 = Color::fromHex(0xfce7f3);
[[gnu::used]] static constexpr Color PINK200 = Color::fromHex(0xfbcfe8);
[[gnu::used]] static constexpr Color PINK300 = Color::fromHex(0xf9a8d4);
[[gnu::used]] static constexpr Color PINK400 = Color::fromHex(0xf472b6);
[[gnu::used]] static constexpr Color PINK500 = Color::fromHex(0xec4899);
[[gnu::used]] static constexpr Color PINK600 = Color::fromHex(0xdb2777);
[[gnu::used]] static constexpr Color PINK700 = Color::fromHex(0xbe185d);
[[gnu::used]] static constexpr Color PINK800 = Color::fromHex(0x9d174d);
[[gnu::used]] static constexpr Color PINK900 = Color::fromHex(0x831843);

[[gnu::used]] static constexpr Color ROSE50 = Color::fromHex(0xfff1f2);
[[gnu::used]] static constexpr Color ROSE100 = Color::fromHex(0xffe4e6);
[[gnu::used]] static constexpr Color ROSE200 = Color::fromHex(0xfecdd3);
[[gnu::used]] static constexpr Color ROSE300 = Color::fromHex(0xfda4af);
[[gnu::used]] static constexpr Color ROSE400 = Color::fromHex(0xfb7185);
[[gnu::used]] static constexpr Color ROSE500 = Color::fromHex(0xf43f5e);
[[gnu::used]] static constexpr Color ROSE600 = Color::fromHex(0xe11d48);
[[gnu::used]] static constexpr Color ROSE700 = Color::fromHex(0xbe123c);
[[gnu::used]] static constexpr Color ROSE800 = Color::fromHex(0x9f1239);
[[gnu::used]] static constexpr Color ROSE900 = Color::fromHex(0x881337);

} // namespace Colors

} // namespace Karm::Gfx
