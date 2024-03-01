#include "pd1/perks_box.h"
#include "panels/info_box.hpp"
#include "control.h"
#include "engine/render/clx_render.hpp"

namespace devilution {

bool PerksBoxIsOpen;

constexpr int fadeLevel = 5;

void DrawPerksBox(const Surface &out)
{
    const Point panelPos = GetLeftPanel().position;
    ClxDraw(out, panelPos + Displacement { 24, 327 }, (*pSTextBoxCels)[0]);    // Draw info-box border

    const Point perksBoxPos = panelPos + Displacement { 27, 28 };

    FillRect(out, perksBoxPos.x, perksBoxPos.y, 265, 297, 0);

	//for (int i = 0; i < fadeLevel; ++i) {
	//	DrawHalfTransparentRectTo(out, perksBoxPos.x, perksBoxPos.y, 265, 297); // Draw info-box background
	//}

}

void CheckPerkBoxClick()
{
    const Rectangle perksBox = { GetLeftPanel().position + Displacement { 24, 25 }, { 271, 303 } };
    if (!perksBox.contains(MousePosition)) {   // Closing box when clicking on charpanel outside the box
        PerksBoxIsOpen = false;
    }
}

}