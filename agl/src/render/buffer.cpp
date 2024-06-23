#include "agl/render/buffer.hpp"

namespace agl
{
index_buffer::index_buffer()
	: m_id{ 0 }
	, m_state{ buffer_state::INVALID }
{
}
index_buffer::index_buffer(buffer data)
	: m_id{ 0 }
	, m_data{ data }
	, m_state{ buffer_state::INVALID }
{
}
void index_buffer::set_id(std::uint64_t id)
{
	set_state(buffer_state::CHANGED);
	m_id = id;
}
std::uint64_t index_buffer::get_id() const
{
	return m_id;
}
void index_buffer::set_state(buffer_state state)
{
	m_state = state;
}
buffer_state index_buffer::get_state() const
{
	return m_state;
}
void index_buffer::clear()
{
	set_state(buffer_state::CHANGED);
	m_data.clear();
}
template <typename TIterator>
void index_buffer::insert(typename buffer::const_iterator pos, TIterator begin, TIterator end)
{
	if (std::distance(begin, end) <= 0)
		return;

	set_state(buffer_state::CHANGED);
	m_data.insert(pos, begin, end);
}
void index_buffer::insert(typename buffer::const_iterator pos, std::uint64_t value)
{
	set_state(buffer_state::CHANGED);
	m_data.insert(pos, value);
}
void index_buffer::set(std::uint64_t index, std::uint64_t value)
{
	set_state(buffer_state::CHANGED);
	m_data[index] = value;
}
std::uint64_t index_buffer::get(std::uint64_t index) const
{
	return m_data[index];
}
void index_buffer::reserve(std::uint64_t n)
{
	m_data.reserve(n);
}
void index_buffer::resize(std::uint64_t n)
{
	set_state(buffer_state::CHANGED);
	m_data.resize(n);
}
std::uint64_t const* const index_buffer::data() const
{
	return m_data.data();
}
vertex_array::vertex_array()
	: m_id{ 0 }
	, m_size{ 0 }
	, m_state{ buffer_state::INVALID }
{
}
vertex_array::vertex_array(mem::pool::allocator<vertex_array> allocator)
	: m_data{ allocator.rebind_copy<std::byte>() }
	, m_id{ 0 }
	, m_row_layout{ allocator.rebind_copy<row_info>() }
	, m_size{ 0 }
	, m_state{ buffer_state::INVALID }
{
}
bool vertex_array::empty() const
{
	return size() == 0;
}
vertex_array::row_info& vertex_array::get_row_info(std::uint64_t row)
{
	return m_row_layout[row];
}
vertex_array::row_info const&vertex_array::get_row_info(std::uint64_t row) const
{
	return m_row_layout[row];
}
std::uint64_t vertex_array::get_id() const
{
	return m_id;
}
void vertex_array::set_id(std::uint64_t id)
{
	set_state(buffer_state::CHANGED);
	m_id = id;
}
std::uint64_t vertex_array::get_buffer_id() const
{
	return m_buffer_id;
}
void vertex_array::set_buffer_id(std::uint64_t id)
{
	m_buffer_id = id;
}
buffer_state vertex_array::get_state() const
{
	return m_state;
}
void vertex_array::set_state(buffer_state state)
{
	m_state = state;
}
void vertex_array::clear()
{
	set_state(buffer_state::CHANGED);
	m_data.clear();
	m_row_layout.clear();
}
std::uint64_t vertex_array::size() const
{
	return m_size;
}
std::uint64_t vertex_array::get_stride_size() const
{
	return get_offset(get_row_layout_size());
}
std::uint64_t vertex_array::get_bytes_size() const
{
	return m_data.size();
}
std::uint64_t vertex_array::get_row_layout_size() const
{
	return m_row_layout.size();
}
std::uint64_t vertex_array::get_offset(std::uint64_t row) const
{
	auto result = std::uint64_t{};
	for(auto i = 0; i < row; ++i)
		result += m_row_layout[i].size;
	return result;
}
void vertex_array::resize(std::uint64_t n)
{
	if (size() == n)
		return;

	set_state(buffer_state::CHANGED);
	m_data.resize(get_stride_size() * n);
	m_size = n;
}
std::byte const* const vertex_array::data() const
{
	return m_data.data();
}
void vertex_array::push_row(render_data_type data_type, bool normalized)
{
	AGL_CORE_ASSERT(get_state() != buffer_state::READY, "buffer must not modify rows count after initialization");

	auto ri = row_info{};
	ri.data_type = data_type;
	ri.size = get_render_data_type_size(data_type);
	ri.var_count = get_render_data_type_count(data_type);
	ri.normalized = normalized;
	m_row_layout.push_back(ri);
}
double vertex_array::get_double(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<double const*>(&m_data[get_offset(row) * index]);
}
glm::dvec2 const& vertex_array::get_dvec2(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::dvec2 const*>(&m_data[get_offset(row) * index]);
}
glm::dvec3 const& vertex_array::get_dvec3(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::dvec3 const*>(&m_data[get_offset(row) * index]);
}
glm::dvec4 const& vertex_array::get_dvec4(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::dvec4 const*>(&m_data[get_offset(row) * index]);
}
float vertex_array::get_float(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<float const*>(&m_data[get_offset(row) * index]);
}
glm::vec2 const& vertex_array::get_vec2(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::vec2 const*>(&m_data[get_offset(row) * index]);
}
glm::vec3 const& vertex_array::get_vec3(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::vec3 const*>(&m_data[get_offset(row) * index]);
}
glm::vec4 const& vertex_array::get_vec4(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::vec4 const*>(&m_data[get_offset(row) * index]);
}
std::uint64_t vertex_array::get_uint64(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<std::uint64_t const*>(&m_data[get_offset(row) * index]);
}
glm::uvec2 const& vertex_array::get_uvec2(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::uvec2 const*>(&m_data[get_offset(row) * index]);
}
glm::uvec3 const& vertex_array::get_uvec3(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::uvec3 const*>(&m_data[get_offset(row) * index]);
}
glm::uvec4 const& vertex_array::get_uvec4(std::uint64_t row, std::uint64_t index) const
{
	return *reinterpret_cast<glm::uvec4 const*>(&m_data[get_offset(row) * index]);
}
void vertex_array::set_double(std::uint64_t row,std::uint64_t index, double value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte*>(&value);
}
void vertex_array::set_dvec2(std::uint64_t row, std::uint64_t index, glm::dvec2 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_dvec3(std::uint64_t row, std::uint64_t index, glm::dvec3 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_dvec4(std::uint64_t row, std::uint64_t index, glm::dvec4 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_float(std::uint64_t row, std::uint64_t index, float value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_vec2(std::uint64_t row, std::uint64_t index, glm::vec2 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_vec3(std::uint64_t row, std::uint64_t index, glm::vec3 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_vec4(std::uint64_t row, std::uint64_t index, glm::vec4 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_uint64(std::uint64_t row,std::uint64_t index,  std::uint64_t value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_uvec2(std::uint64_t row, std::uint64_t index, glm::uvec2 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_uvec3(std::uint64_t row, std::uint64_t index, glm::uvec3 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}
void vertex_array::set_uvec4(std::uint64_t row, std::uint64_t index, glm::uvec4 const& value)
{
	set_state(buffer_state::CHANGED);
	m_data[get_offset(row) * index] = *reinterpret_cast<std::byte const*>(&value);
}


index_buffer& render_object::get_index_buffer()
{
	return m_index_buffer;
}
index_buffer const& render_object::get_index_buffer() const
{
	return m_index_buffer;
}
vertex_array& render_object::get_vertex_array()
{
	return m_vertex_array;
}
vertex_array const& render_object::get_vertex_array() const
{
	return m_vertex_array;
}
}