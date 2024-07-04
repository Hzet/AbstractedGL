#pragma once
#include <glm/glm.hpp>
#include "agl/memory/vector.hpp"
#include "agl/render/render-data-type.hpp"

namespace agl
{
enum class buffer_state
{
	INVALID,
	CHANGED,
	READY
};

class index_buffer
{
	using buffer = mem::vector<std::uint64_t>;
public:
						         index_buffer();
						         index_buffer(buffer data);
								 index_buffer(mem::pool::allocator<std::uint64_t> allocator);
	void                         clear();         
	std::uint64_t const* const   data() const;
	std::uint64_t                get(std::uint64_t index) const;
	std::uint64_t                get_bytes_size() const;
	std::uint64_t                get_id() const;
	std::uint64_t                get_size() const;
	buffer_state                 get_state() const;
    template <typename TIterator>  
	void                         insert(buffer::const_iterator pos, TIterator begin, TIterator end);
	void                         insert(buffer::const_iterator pos, std::uint64_t value);
	void                         reserve(std::uint64_t n);
	void                         resize(std::uint64_t n);
	void                         set(std::uint64_t index, std::uint64_t value);
	void                         set_id(std::uint64_t id);
	void                         set_state(buffer_state state);

private:
	std::uint64_t m_id;
	buffer        m_data;
	buffer_state  m_state;
};

/*
* example:
* buffer layout: vec2, vec3, uint, uvec2
* buffer row[0] = vec2, row[2] = uint
* size() = vertex_buffer.size()
* 
* makes a layered cake pattern from the pushed rows
* all rows must be of the same size
*/
class vertex_array
{
public:
	// descripts the layout of the data stored in the vertex buffer
	struct row_info
	{
		std::uint64_t size; // ovarall size of descripted data
		std::uint64_t var_count;
		bool normalized;
		render_data_type data_type;
	};

public:
	                       vertex_array();
	                       vertex_array(mem::pool::allocator<vertex_array> allocator);
	void                   clear();
	std::byte const* const data() const;
	bool                   empty() const;
	std::uint64_t          get_buffer_id() const;
	std::uint64_t          get_bytes_size() const; // size of the buffer in bytes
	double                 get_double(std::uint64_t row, std::uint64_t index) const; 
	glm::dvec2 const&      get_dvec2(std::uint64_t row, std::uint64_t index) const;
	glm::dvec3 const&      get_dvec3(std::uint64_t row, std::uint64_t index) const;
	glm::dvec4 const&      get_dvec4(std::uint64_t row, std::uint64_t index) const;
	float                  get_float(std::uint64_t row, std::uint64_t index) const;
	std::uint64_t          get_id() const;
	std::uint64_t          get_offset(std::uint64_t row) const;
	row_info&              get_row_info(std::uint64_t row);
	row_info const&        get_row_info(std::uint64_t row) const;
	std::uint64_t          get_row_layout_size() const; // count of different rows
	std::uint64_t          get_size() const; // count of elements in one row
	buffer_state           get_state() const;
	void                   set_state(buffer_state state);
	std::uint64_t          get_stride_size() const; // sum of sizes of all rows data types
	std::uint64_t          get_uint64(std::uint64_t row, std::uint64_t index) const;
	glm::uvec2 const&      get_uvec2(std::uint64_t row, std::uint64_t index) const;
	glm::uvec3 const&      get_uvec3(std::uint64_t row, std::uint64_t index) const;
	glm::uvec4 const&      get_uvec4(std::uint64_t row, std::uint64_t index) const;
	glm::vec2 const&       get_vec2(std::uint64_t row, std::uint64_t index) const;
	glm::vec3 const&       get_vec3(std::uint64_t row, std::uint64_t index) const;
	glm::vec4 const&       get_vec4(std::uint64_t row, std::uint64_t index) const;
	void                   push_row(render_data_type data_type, bool normalized = false); 
	void                   resize(std::uint64_t n);
	void                   set_buffer_id(std::uint64_t id);
    template <typename TIterator>	
	void                   set_double(std::uint64_t row, TIterator first, TIterator last);
	void                   set_double(std::uint64_t row, std::uint64_t index, double value);
    template <typename TIterator>	
    void                   set_dvec2(std::uint64_t row, TIterator first, TIterator last);
	void                   set_dvec2(std::uint64_t row, std::uint64_t index, glm::dvec2 const& value);
    template <typename TIterator>	
    void                   set_dvec3(std::uint64_t row, TIterator first, TIterator last);
	void                   set_dvec3(std::uint64_t row, std::uint64_t index, glm::dvec3 const& value);
    template <typename TIterator>	
    void                   set_dvec4(std::uint64_t row, TIterator first, TIterator last);
	void                   set_dvec4(std::uint64_t row, std::uint64_t index, glm::dvec4 const& value);
    template <typename TIterator>	
	void                   set_float(std::uint64_t row, TIterator first, TIterator last);
	void                   set_float(std::uint64_t row, std::uint64_t index, float value);
	void                   set_id(std::uint64_t id);
    template <typename TIterator>	
    void                   set_uint64(std::uint64_t row, TIterator first, TIterator last);
	void                   set_uint64(std::uint64_t row, std::uint64_t index, std::uint64_t value);
    template <typename TIterator>	
    void                   set_uvec2(std::uint64_t row, TIterator first, TIterator last);
	void                   set_uvec2(std::uint64_t row, std::uint64_t index, glm::uvec2 const& value);
    template <typename TIterator>	
    void                   set_uvec3(std::uint64_t row, TIterator first, TIterator last);
	void                   set_uvec3(std::uint64_t row, std::uint64_t index, glm::uvec3 const& value);
    template <typename TIterator>	
    void                   set_uvec4(std::uint64_t row, TIterator first, TIterator last);
	void                   set_uvec4(std::uint64_t row, std::uint64_t index, glm::uvec4 const& value);
    template <typename TIterator>	
    void                   set_vec2(std::uint64_t row, TIterator first, TIterator last);
	void                   set_vec2(std::uint64_t row, std::uint64_t index, glm::vec2 const& value);
    template <typename TIterator>	
    void                   set_vec3(std::uint64_t row, TIterator first, TIterator last);
	void                   set_vec3(std::uint64_t row, std::uint64_t index, glm::vec3 const& value);
    template <typename TIterator>	
    void                   set_vec4(std::uint64_t row, TIterator first, TIterator last);
	void                   set_vec4(std::uint64_t row, std::uint64_t index, glm::vec4 const& value);

private:
	std::uint64_t          m_buffer_id;
	mem::vector<std::byte> m_data;
	std::uint64_t          m_id;
	mem::vector<row_info>  m_row_layout;
	std::uint64_t          m_size;
	buffer_state           m_state;
};

class render_object
{
public:
	                    render_object(mem::pool::allocator<render_object> allocator = {});
	std::uint64_t       get_bytes_size() const;
	index_buffer&       get_index_buffer();
	index_buffer const& get_index_buffer() const;
	vertex_array&       get_vertex_array();
	vertex_array const& get_vertex_array() const;

private:
	index_buffer m_index_buffer;
	vertex_array m_vertex_array;
};




template <typename TIterator>
void vertex_array::set_double(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for(firstauto i = 0 first != last; ++first, ++i)
		set_double(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_dvec2(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_dvec2(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_dvec3(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_dvec3(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_dvec4(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_dvec4(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_float(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_float(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_uint64(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_uint64(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_uvec2(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_uvec2(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_uvec3(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_uvec3(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_uvec4(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_uvec4(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_vec2(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_vec2(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_vec3(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_vec3(row, i, *first);
}
template <typename TIterator>
void vertex_array::set_vec4(std::uint64_t row, TIterator first, TIterator last)
{
	if(auto size = std::distance(first, last); 
       size > get_size())
		resize(size);

	set_state(buffer_state::CHANGED);
	for (auto i = 0; first != last; ++first, ++i)
		set_vec4(row, i, *first);
}






}