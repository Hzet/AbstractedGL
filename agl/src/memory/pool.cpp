#include "agl/memory/pool.hpp"
#include "agl/core/logger.hpp"
#include "agl/util/util.hpp"

namespace agl
{
namespace mem
{
pool::pool()
	: m_memory{ nullptr }
	, m_occupancy{ 0 }
	, m_size{ 0 }
{
}
pool::pool(pool&& other)
	: m_free_spaces{ std::move(other.m_free_spaces) }
	, m_memory{ other.m_memory }
	, m_occupancy{ other.m_occupancy}
	, m_occupied_spaces{ std::move(other.m_occupied_spaces) }
	, m_size{ other.m_size }
{
	other.m_memory = nullptr;
}
pool& pool::operator=(pool&& other)
{
	if (&other == this)
		return *this;

	m_free_spaces = std::move(other.m_free_spaces);
	m_memory = other.m_memory;
	m_occupancy = other.m_occupancy;
	m_occupied_spaces = std::move(other.m_occupied_spaces);
	m_size = other.m_size;
	other.m_memory = nullptr;
	return *this;
}
pool::~pool() noexcept
{
	destroy();
}

std::byte* pool::allocate(std::uint64_t size, std::uint64_t alignment)
{
	AGL_ASSERT(!full(), "the pool is at it's maximum capacity");

	auto space = pop_free_space(size);
	auto i = size;
	std::align(alignment, size, reinterpret_cast<void*&>(space.ptr), i);

	AGL_ASSERT(space.ptr != nullptr, "invalid pointer after alignment");
	m_occupancy += space.size;
	push_occupied_space(space);
	return space.ptr;
}
void pool::create(std::uint64_t size)
{
	AGL_ASSERT(size != 0, "cannot allocate pool of 0 bytes");

	m_memory = reinterpret_cast<std::byte*>(std::malloc(size));
	
	if (m_memory == nullptr)
		throw std::exception{ logger::combine_message("not enough memory to allocate pool of {} bytes", size).c_str() };

	m_size = size;
	push_free_space({ m_memory, m_size });
}
void pool::deallocate(std::byte* ptr, std::uint64_t count)
{
	auto found = find_occupied_space(ptr);
	auto space = *found;
	m_occupancy -= space.size;
	m_occupied_spaces.erase(found);
	push_free_space(std::move(space));
}
void pool::destroy()
{
	if (m_memory == nullptr)
		return;

	AGL_ASSERT(m_occupied_spaces.empty(), "some objects were not deallocated");

	std::free(m_memory);
	m_size = 0;
	m_occupancy = 0;
	m_memory = nullptr;
	m_free_spaces.clear();
	m_occupied_spaces.clear();
}
bool pool::full() const
{
	return m_occupancy == m_size;
}
bool pool::empty() const
{
	return m_occupancy == 0;
}
std::uint64_t pool::occupancy() const
{
	return m_occupancy;
}
std::uint64_t pool::size() const
{
	return m_size;
}
bool pool::has_pointer(std::byte* ptr) const
{
	return m_memory <= ptr && ptr < m_memory + m_size;
}
pool::space pool::pop_free_space(std::uint64_t size)
{
	auto found = find_free_space(size);
	AGL_ASSERT(found != m_free_spaces.cend(), "insufficient memory to allocate this object");

	auto space = *found;
	m_free_spaces.erase(found);

	if (space.size > size)
	{
		auto excess = space.size - size;
		space.size -= excess;
		push_free_space(pool::space{ space.ptr + size, excess });
	}
	
	return space;
}
void pool::push_free_space(pool::space space)
{
	auto it = find_free_space(space.size);
	m_free_spaces.insert(it, space);
}
vector<pool::space>::const_iterator pool::find_free_space(std::uint64_t size) const
{
	auto comp = [](pool::space const& space, std::uint64_t size)
		{
			return size >= space.size;
		};

	return std::lower_bound(m_free_spaces.cbegin(), m_free_spaces.cend(), size, comp);
}
vector<pool::space>::const_iterator pool::find_occupied_space(std::byte* ptr) const
{
	auto comp = [](pool::space const& space, std::byte* ptr)
		{
			return ptr < space.ptr;
		};

	auto found = std::lower_bound(m_occupied_spaces.cbegin(), m_occupied_spaces.cend(), ptr, comp);
	return found;
}
void pool::pop_occupied_space(std::byte* ptr)
{
	auto it = find_occupied_space(ptr);
	AGL_ASSERT(it != m_occupied_spaces.cend(), "no data allocated at given pointer location");
	AGL_ASSERT(it->ptr == ptr, "no data allocated at given pointer location");
	m_occupied_spaces.erase(it);
}
void pool::push_occupied_space(pool::space space)
{
	auto it = find_occupied_space(space.ptr);
	m_occupied_spaces.insert(it, space);
}
bool pool::free_space_comparator(std::uint64_t size, pool::space const& space)
{
	return size < space.size;
}
bool pool::occupied_space_comparator(std::byte* ptr, pool::space const& space)
{
	return ptr < space.ptr;
}
void pool::on_attach(application* app)
{
	auto* logger = app->get_resource<agl::logger>();
	create(10 * 1024 * 1024);
	logger->debug("Pool at {} | {}: OK", m_memory, util::ns::memory_size(size()));
}
void pool::on_detach(application* app)
{
	auto* logger = app->get_resource<agl::logger>();
	logger->debug("Pool at {} | {}: OFF", m_memory, util::ns::memory_size(size()));
	destroy();
}
void pool::on_update(application* app)
{
}
}
}