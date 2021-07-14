#ifndef GOOST_H
#define GOOST_H

#include "core/goost_engine.h"
#include "core/image/goost_image.h"
#include "core/image/goost_image_bind.h"
#include "core/image/image_blender.h"
#include "core/image/image_indexed.h"
#include "core/invoke_state.h"
#include "core/math/geometry/2d/goost_geometry_2d.h"
#include "core/math/geometry/2d/goost_geometry_2d_bind.h"
#include "core/math/geometry/2d/poly/boolean/poly_boolean.h"
#include "core/math/geometry/2d/poly/decomp/poly_decomp.h"
#include "core/math/geometry/2d/poly/offset/poly_offset.h"
#include "core/math/geometry/2d/poly/poly_backends.h"
#include "core/math/geometry/2d/random_2d.h"
#include "core/math/random.h"
#include "core/script/mixin_script/mixin_script.h"
#include "core/types/linked_list.h"
#include "core/types/variant_map.h"
#include "core/types/variant_resource.h"

#include "scene/2d/editor/poly_node_2d_editor_plugin.h"
#include "scene/2d/editor/visual_shape_2d_editor_plugin.h"
#include "scene/2d/poly_generators_2d.h"
#include "scene/2d/poly_shape_2d.h"
#include "scene/2d/visual_shape_2d.h"
#include "scene/gui/grid_rect.h"
#include "scene/main/stopwatch.h"
#include "scene/physics/2d/poly_collision_shape_2d.h"
#include "scene/physics/2d/shape_cast_2d.h"
#include "scene/resources/gradient_texture_2d.h"
#include "scene/resources/light_texture.h"

namespace goost {
template <typename T>
void register_class() {
	ClassDB::register_class<T>();
}
} // namespace goost

#endif // GOOST_H
