//Original belonged to Jeremy Hartd
//Modified by Cory Jensen

#include "camera.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
mat4 Camera::view()
{
  vec3 right = glm::normalize(cross(cameraFront, vec3(0.0f, 1.0f, 0.0f)));
  cameraUp = glm::normalize(cross(right, cameraFront));
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
