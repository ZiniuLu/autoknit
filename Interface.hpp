#pragma once

#include <kit/kit.hpp>

#include <kit/GLBuffer.hpp>
#include <kit/GLVertexArray.hpp>

#include "pipeline.hpp"

struct Interface : public kit::Mode {
	Interface();
	virtual ~Interface();
	virtual void update(float elapsed) override;
	virtual void draw() override;
	virtual void pointer_action(kit::PointerID pointer, kit::PointerAction action, kit::Pointer const &old_state, kit::Pointer const &new_state) override;

	//framebuffers + textures
	glm::uvec2 fb_size = glm::uvec2(0);

	GLuint color_id_fb = 0; //(color_tex, id_tex) + depth_tex
	GLuint color_tex = 0;
	GLuint id_tex = 0;
	GLuint depth_tex = 0; //depth values (for soft edges)

	void alloc_fbs(); //(re-)allocate framebuffers given current display size

	//camera:
	struct Camera {
		glm::vec3 center = glm::vec3(0.0f);
		float radius = 5.0f;
		//all in radians:
		float azimuth = glm::radians(30.0f);
		float elevation = glm::radians(45.0f);
		float fovy = glm::radians(60.0f);

		//matrix that takes positions to camera space:
		glm::mat4 mv() const {
			float ca = std::cos(azimuth);
			float sa = std::sin(azimuth);
			float ce = std::cos(elevation);
			float se = std::sin(elevation);
			glm::vec3 right = glm::vec3(     -sa,      ca, 0.0f);
			glm::vec3 up    = glm::vec3(-se * ca,-se * sa, ce);
			glm::vec3 out   = glm::vec3( ce * ca, ce * sa, se);
			glm::vec3 at = out * radius;
			return glm::mat4(
				right.x, up.x, out.x, 0.0f,
				right.y, up.y, out.y, 0.0f,
				right.z, up.z, out.z, 0.0f,
				glm::dot(-at, right), glm::dot(-at, up), glm::dot(-at, out), 1.0f
			);
		}
		glm::mat4 mvp() const {
			return glm::infinitePerspective(fovy, kit::display.aspect, 0.1f) * mv();
		}
	} camera;

	//mouse tracking:
	enum Drag {
		DragNone = 0,
		DragCamera,
		DragCameraFlipX, //for dragging when upside-down
	};
	Drag drag = DragNone;

	struct {
		glm::vec2 at = glm::vec2(std::numeric_limits< float >::quiet_NaN());
		bool moved = false;
	} mouse;

	struct {
		glm::vec3 point = glm::vec3(std::numeric_limits< float >::quiet_NaN());
		uint32_t tri = -1U;

		void clear() {
			tri = -1U;
		}
	} hovered;

	void update_hovered();

	//original model:
	ak::Model model;
	//model buffer: (vertices, normals, ids)
	GLAttribBuffer< glm::vec3, glm::vec3, glm::u8vec4 > model_triangles;
	GLVertexArray model_triangles_for_model_draw;
	void set_model(ak::Model const &new_model);

	//place camera to view whole model:
	void reset_camera();

};
