#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <Renderer/LightingProperties.h>
#include <Renderer/Shader.h>

class Camera;

class GameObject {
public:

	const inline glm::vec3& scale()		const { return m_scale; }
	const inline glm::vec3& location()	const { return m_location; }
	const inline glm::vec3& rotation()	const { return m_rotation; }
	inline glm::vec3& raw_scale() { return m_scale; }
	inline glm::vec3& raw_location() { return m_location; }
	inline glm::vec3& raw_rotation() { return m_rotation; }

	void calculate_model_matrix_if_needed();

	const inline bool should_recalculate_model_matrix() const { return m_should_recalculate_model_matrix; }
	void set_should_recalculate_model_matrix(bool should) { m_should_recalculate_model_matrix = should; }
	
	const glm::mat4& model();
	const glm::mat4& model() const;

	void set_scale(const glm::vec3&);
	void set_location(const glm::vec3&);
	void set_rotation(const glm::vec4&);

	const bool constant_model() const { return m_constant_model; }
	void set_constant_model(const bool b) { m_constant_model = b; }

	const glm::mat4 constant_model_matrix() const { return m_constant_model_matrix; }
	void set_constant_model_matrix(const glm::mat4 b) { m_constant_model_matrix = b; }

	void move(const glm::vec3&);

private:
	friend class WorldObject;

	GameObject();

	bool m_should_recalculate_model_matrix{ true };

	bool m_constant_model{ false };
	glm::mat4 m_constant_model_matrix{ glm::mat4(1.0f) };
	glm::mat4 m_model{1.0f};
	
	glm::vec3 m_scale{1.0f};
	glm::vec3 m_location{0.0f};
	glm::vec3 m_rotation{0.0f};
};

class WorldObject {
public:

	WorldObject() {}
	virtual ~WorldObject() = default;

	GameObject& game_object() { return m_game_object; };
	const GameObject& game_object() const { return m_game_object; }

private:

	GameObject m_game_object;
};

class RenderedWorldObject : public WorldObject {

public:
	RenderedWorldObject() {}
	virtual ~RenderedWorldObject() override = default;
	virtual void render(const Camera& camera, const glm::mat4& proj) const = 0;

private:

};

class SingleColoredWorldObject : public RenderedWorldObject
{
public:
	SingleColoredWorldObject();
	SingleColoredWorldObject(glm::vec3 color);
	virtual ~SingleColoredWorldObject() override = default;

	static inline Shader& shader() { return m_shader; }
	virtual void render(const Camera& camera, const glm::mat4& proj) const override;

	void set_color(const glm::vec3&);
	const inline glm::vec3 color() const { return m_color; };

private:
	glm::vec4 m_color{ 1.0f };

	static Shader m_shader;
};

class SingleColoredSlyWorldObject : public SingleColoredWorldObject
{
public:
	SingleColoredSlyWorldObject() : SingleColoredWorldObject() {}
	SingleColoredSlyWorldObject(glm::vec3 color) : SingleColoredWorldObject(color) {}
	virtual ~SingleColoredSlyWorldObject() {}

	static inline Shader& shader() { return m_shader; }

    void set_additional_model_matrix(const glm::mat4& mat);
	virtual void render(const Camera& camera, const glm::mat4& proj) const override;
private:

	static Shader m_shader;

    glm::mat4 m_additional_model{ 1.0f };

    const inline static auto rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(3*90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });
};


class LightedWorldObject : public RenderedWorldObject
{
public:

	virtual ~LightedWorldObject() override = default;
	virtual void render(const Camera& camera, const glm::mat4& proj) const override = 0;

	const inline LightingProperties lighting_properties() const { return m_lighting_properties; };

private:
	LightingProperties m_lighting_properties;
};

