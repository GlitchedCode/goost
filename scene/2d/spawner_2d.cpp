#include "spawner_2d.h"

#include "core/engine.h"
#include "scene/resources/packed_scene.h"

void Spawner2D::set_resource(const Ref<Resource> &p_resource) {
	const Ref<PackedScene> &scene = p_resource;
	const Ref<Script> &script = p_resource;

	if (scene.is_valid()) {
		resource = scene;
	} else if (script.is_valid()) {
		resource = script;
	} else {
		resource = Ref<Resource>();
		if (p_resource.is_valid()) {
			ERR_FAIL_MSG("Resource is not a PackedScene nor a Script");
		}
	}
}

void Spawner2D::set_enabled(bool p_enabled) {
	_set_process(p_enabled);

	delay_time = 0.0;
	time = 0.0;
	amount = 0;

	if (p_enabled && delay <= 0.0) {
		spawn();
	}
}

Node *Spawner2D::spawn() {
#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		return nullptr;
	}
#endif
	ERR_FAIL_COND_V_MSG(!is_inside_tree(), nullptr,
			"Spawner2D must be added to the SceneTree in order to spawn nodes");

	const Ref<PackedScene> &scene = resource;
	const Ref<Script> &script = resource;

	Node *node = nullptr;

	if (scene.is_valid()) {
		node = scene->instance();

	} else if (script.is_valid()) {
		StringName base_type = script->get_instance_base_type();
		bool valid = !ClassDB::is_parent_class(base_type, "Node");
		ERR_FAIL_COND_V_MSG(valid, nullptr, "Script does not inherit a Node: " + script->get_path());

		Object *obj = ClassDB::instance(base_type);
		node = Object::cast_to<Node>(obj);
		Variant s = script;
		node->set_script(s);
	} else {
		ERR_FAIL_V_MSG(nullptr, "Could not spawn an object: the resource is not a scene nor a script.");
	}

	add_child(node);
	amount += 1;

	return node;
}

void Spawner2D::set_rate(int p_rate) {
	ERR_FAIL_COND(p_rate < 1);
	rate = p_rate;
}

void Spawner2D::set_step(double p_step) {
	ERR_FAIL_COND(p_step <= 0.0);
	step = p_step;
}

void Spawner2D::set_delay(double p_delay) {
	ERR_FAIL_COND(p_delay < 0.0);
	delay = p_delay;
}

// void Spawner2D::set_lifetime(double p_lifetime) {
// 	ERR_FAIL_COND(p_lifetime < 0.0);
// 	lifetime = p_lifetime;
// }

void Spawner2D::set_limit(int p_limit) {
	ERR_FAIL_COND(p_limit < 0);
	limit = p_limit;
}

void Spawner2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (enabled) {
				set_enabled(true);
			}
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (!enabled || process_mode == PROCESS_PHYSICS || !is_processing_internal()) {
				return;
			}
			delay_time += get_process_delta_time();
			time += get_process_delta_time();
			_process_spawn();
		} break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (!enabled || process_mode == PROCESS_IDLE || !is_physics_processing_internal()) {
				return;
			}
			delay_time += get_physics_process_delta_time();
			time += get_physics_process_delta_time();
			_process_spawn();
		} break;
	}
}

void Spawner2D::_process_spawn() {
	if (delay > 0.0) {
		if (delay_time < delay) {
			return;
		}
	}
	if (time > (step / rate)) {
		spawn();
		time = 0.0;

		if (limit > 0 && amount >= limit) {
			set_enabled(false);
		}
	}
}

void Spawner2D::set_process_mode(ProcessMode p_mode) {
	if (process_mode == p_mode) {
		return;
	}
	switch (process_mode) {
		case PROCESS_PHYSICS:
			if (is_physics_processing_internal()) {
				set_physics_process_internal(false);
				set_process_internal(true);
			}
			break;
		case PROCESS_IDLE:
			if (is_processing_internal()) {
				set_process_internal(false);
				set_physics_process_internal(true);
			}
			break;
	}
	process_mode = p_mode;
}

void Spawner2D::_set_process(bool p_process) {
	switch (process_mode) {
		case PROCESS_PHYSICS: {
			set_physics_process_internal(p_process);
		} break;
		case PROCESS_IDLE: {
			set_process_internal(p_process);
		} break;
	}
	enabled = p_process;
}

void Spawner2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_resource", "object"), &Spawner2D::set_resource);
	ClassDB::bind_method(D_METHOD("get_resource"), &Spawner2D::get_resource);

	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &Spawner2D::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &Spawner2D::is_enabled);

	ClassDB::bind_method(D_METHOD("spawn"), &Spawner2D::spawn);

	ClassDB::bind_method(D_METHOD("set_rate", "rate"), &Spawner2D::set_rate);
	ClassDB::bind_method(D_METHOD("get_rate"), &Spawner2D::get_rate);

	ClassDB::bind_method(D_METHOD("set_step", "step"), &Spawner2D::set_step);
	ClassDB::bind_method(D_METHOD("get_step"), &Spawner2D::get_step);

	ClassDB::bind_method(D_METHOD("set_delay", "delay"), &Spawner2D::set_delay);
	ClassDB::bind_method(D_METHOD("get_delay"), &Spawner2D::get_delay);

	// ClassDB::bind_method(D_METHOD("set_lifetime", "lifetime"), &Spawner2D::set_lifetime);
	// ClassDB::bind_method(D_METHOD("get_lifetime"), &Spawner2D::get_lifetime);

	ClassDB::bind_method(D_METHOD("set_limit", "limit"), &Spawner2D::set_limit);
	ClassDB::bind_method(D_METHOD("get_limit"), &Spawner2D::get_limit);

	ClassDB::bind_method(D_METHOD("set_process_mode", "process_mode"), &Spawner2D::set_process_mode);
	ClassDB::bind_method(D_METHOD("get_process_mode"), &Spawner2D::get_process_mode);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene,Script"), "set_resource", "get_resource");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rate", PROPERTY_HINT_RANGE, "1,20,1,or_greater"), "set_rate", "get_rate");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "step", PROPERTY_HINT_RANGE, "0.1,60.0,0.1,or_greater"), "set_step", "get_step");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "delay", PROPERTY_HINT_RANGE, "0.0,60.0,0.1,or_greater"), "set_delay", "get_delay");
	// ADD_PROPERTY(PropertyInfo(Variant::REAL, "lifetime", PROPERTY_HINT_RANGE, "0.0,5.0,0.1,or_greater"), "set_lifetime", "get_lifetime");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "limit", PROPERTY_HINT_RANGE, "0,20,1,or_greater"), "set_limit", "get_limit");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "process_mode", PROPERTY_HINT_ENUM, "Physics,Idle"), "set_process_mode", "get_process_mode");

	BIND_ENUM_CONSTANT(PROCESS_PHYSICS);
	BIND_ENUM_CONSTANT(PROCESS_IDLE);
}
