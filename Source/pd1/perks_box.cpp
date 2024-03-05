#include "pd1/perks_box.h"
#include "panels/info_box.hpp"
#include "panels/charpanel.hpp"
#include "control.h"
#include "engine/render/clx_render.hpp"

namespace devilution {

bool PerksBoxIsOpen;

constexpr int fadeLevel = 4;

// Two alternative ways to draw perks list
void DrawListWithButtons(const Surface &out, Rectangle &rect);
void DrawListWithPluses(const Surface &out, Rectangle &rect);
unsigned int temp = 1;

void DrawPerksBox(const Surface &out)
{
    const Point panelPos = GetLeftPanel().position;
    const Point perksBoxPos = panelPos + Displacement { 27, 28 };

    //FillRect(out, perksBoxPos.x, perksBoxPos.y, 265, 297, 0);                  // Draw info-box background
    ClxDraw(out, panelPos + Displacement { 24, 327 }, (*pSTextBoxCels)[0]);    // Draw info-box border

	for (int i = 0; i < fadeLevel; ++i) {
		DrawHalfTransparentRectTo(out, perksBoxPos.x, perksBoxPos.y, 265, 297); // Draw info-box background
	}

    // Drawing header ============
    Rectangle rect = { perksBoxPos + Displacement { 8, 12 }, { 248, 0 } };
    DrawString(out, _("Perks points:"), rect, { .flags = UiFlags::None });
    DrawString(out, _("3"), rect, { .flags = UiFlags::AlignRight | UiFlags::ColorRed });        //TODO: here will be actual number of perk-points available

    // Drawing divider ===========
    const Rectangle dividerLineRect = { panelPos + Displacement { 26, 25 }, { 267, 3 } };
	out.BlitFrom(out, MakeSdlRect(dividerLineRect), dividerLineRect.position + Displacement { 0, 2 * 12 + 13 });  // 2 lines 12px each + margin (?)

    // Drawing perks list ========
    temp % 2 ? DrawListWithButtons(out, rect) : DrawListWithPluses(out, rect);


    // Drawing divider bottom ===========
	out.BlitFrom(out, MakeSdlRect(dividerLineRect), dividerLineRect.position + Displacement { 0, 297 - (2 * 12 + 8) });
    DrawString(out, _("Close"), { panelPos + Displacement { 35, 300 }, { 248, 0 } }, { .flags = UiFlags::AlignCenter | UiFlags::ColorWhite });

}

void CheckPerkBoxClick()
{
    const Rectangle perksBox = { GetLeftPanel().position + Displacement { 24, 25 }, { 271, 303 } };
    const Rectangle closeButton = { GetLeftPanel().position + Displacement { 24, 293 }, { 248, 30 } };
    if (!perksBox.contains(MousePosition) || closeButton.contains(MousePosition)) {   // Closing box when clicking on charpanel outside the box
        PerksBoxIsOpen = false;
        return;
    }

    temp++;
}


void DrawListWithButtons(const Surface &out, Rectangle &rect)
{
    rect.position.y += 40;
    //Test data:
    DrawString(out, "Quick learner [0/15]", rect, { .flags = UiFlags::ColorWhite });
    ClxDrawTRN(out, { rect.position.x + 207, rect.position.y + 18}, (*pChrButtons)[1], LightTables[1].data());  //example for light tables
    rect.position.y += 24;
    DrawString(out, "Stone skin [1/15]", rect, { .flags = UiFlags::ColorBlue });
    ClxDrawTRN(out, { rect.position.x + 207, rect.position.y + 18}, (*pChrButtons)[1], LightTables[1].data());
    rect.position.y += 24;
    DrawString(out, "Mace mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    ClxDrawTRN(out, { rect.position.x + 207, rect.position.y + 18}, (*pChrButtons)[1], LightTables[1].data());
    rect.position.y += 24;
    DrawString(out, "Axe mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    ClxDrawTRN(out, { rect.position.x + 207, rect.position.y + 18}, (*pChrButtons)[1], LightTables[1].data());
    rect.position.y += 24;
    DrawString(out, "Bow mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    ClxDrawTRN(out, { rect.position.x + 207, rect.position.y + 18}, (*pChrButtons)[1], LightTables[1].data());
    rect.position.y += 24;
    DrawString(out, "Singer [1/1]", rect, { .flags = UiFlags::None });
    rect.position.y += 24;
    DrawString(out, "Necromancy [0/3]", rect, { .flags = UiFlags::ColorRed });
    DrawString(out, "Req: Lvl 33", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight });
    rect.position.y += 24;
    DrawString(out, "Sword mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    ClxDrawTRN(out, { rect.position.x + 207, rect.position.y + 18}, (*pChrButtons)[1], LightTables[1].data());
}

void DrawListWithPluses(const Surface &out, Rectangle &rect)
{
    rect.position.y += 40;
    //Test data: try ☩ char
    DrawString(out, "Quick learner [0/15]", rect, { .flags = UiFlags::ColorWhite });
    DrawString(out, "+", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight | UiFlags::FontSize30 });
    rect.position.y += 24;
    DrawString(out, "Stone skin [1/15]", rect, { .flags = UiFlags::ColorBlue });
    DrawString(out, "+", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight | UiFlags::FontSize30 });
    rect.position.y += 24;
    DrawString(out, "Mace mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    DrawString(out, "+", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight | UiFlags::FontSize30 });
    rect.position.y += 24;
    DrawString(out, "Axe mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    DrawString(out, "+", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight | UiFlags::FontSize30 });
    rect.position.y += 24;
    DrawString(out, "Bow mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    DrawString(out, "+", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight | UiFlags::FontSize30 });
    rect.position.y += 24;
    DrawString(out, "Singer [1/1]", rect, { .flags = UiFlags::None });
    rect.position.y += 24;
    DrawString(out, "Necromancy [0/3]", rect, { .flags = UiFlags::ColorRed });
    DrawString(out, "Req: Lvl 33", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight });
    rect.position.y += 24;
    DrawString(out, "Sword mastery [0/15]", rect, { .flags = UiFlags::ColorWhite });
    DrawString(out, "+", rect, { .flags = UiFlags::ColorRed | UiFlags::AlignRight | UiFlags::FontSize30 });
}

}