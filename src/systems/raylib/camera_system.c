#include "camera_system.h"
#include "../../game/scene.h"
#include "../../components/transform.h"
#include "../../ecs/ecs.h"

void camera_system_update(void) {
    const Transform2D *t = GET_COMPONENT(transform, player_entity);

    game_camera.target.x = t->position.x;
    game_camera.target.y = t->position.y;
}
