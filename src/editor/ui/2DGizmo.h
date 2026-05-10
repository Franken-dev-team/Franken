#pragma once

class Gizmo {
	public:
		static void Render(int center_x, int center_y);

	private:
		static void drawCircle(int center_x, int center_y);
		static void drawArrow(int center_x, int center_y);
};
