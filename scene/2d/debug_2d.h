#pragma once

#include "core/engine.h"
#include "core/resource.h"
#include "scene/2d/node_2d.h"

class DebugCapture;

class Debug2D : public Node {
	GDCLASS(Debug2D, Node);

private:
	static Debug2D *singleton;

	Node2D *base = nullptr;
	ObjectID canvas_item; // Currently used item passed to draw commands.

	struct DrawCommand {
		enum Type {
			POLYLINE,
		};
		ObjectID canvas_item;
		Type type;
		Vector<Variant> args;
	};
	Vector<DrawCommand> commands;
	Ref<DebugCapture> state;
	int capture_begin = 0;
	int capture_end = 0;

protected:
	static void _bind_methods();

	void _on_canvas_item_draw(Object *p_item);
	void _push_command(DrawCommand &p_command);
	void _draw_command(const DrawCommand &p_command, CanvasItem *p_item);

public:
	static Debug2D *get_singleton() { return singleton; }

	void set_canvas_item(Object* p_canvas_item);
	Object* get_canvas_item() const;
	Object* get_base() const { return base; } 

	void draw_polyline(const Vector<Point2> &p_polyline, const Color &p_color, real_t p_width = 1.0);

	void capture();
	Ref<DebugCapture> get_capture() const { return state; }

	void update();
	void clear();

	Debug2D();
};

class DebugCapture : public Reference {
	GDCLASS(DebugCapture, Reference);

	friend class Debug2D;

protected:
	static void _bind_methods();

	Vector<int> snapshots;
	int snapshot = -1;
	bool accumulate = true;

public:
	void draw(int p_index);
	void draw_next();
	void draw_prev();

	int get_count() { return snapshots.size() / 2; }

	void set_accumulate(bool p_accumulate);
	bool is_accumulating() const { return accumulate; }

	void reset();
};
