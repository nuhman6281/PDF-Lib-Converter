/**
 * @file memory_utils.cpp
 * @brief Memory utility functions implementation
 */

#include "memory_utils.h"
#include <cstring>
#include <cstdlib>
#include <new>

#ifdef _WIN32
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace PDFLib {
namespace Utils {

// Memory statistics (thread-unsafe, for debugging only)
static size_t g_total_allocated = 0;
static size_t g_total_freed = 0;
static size_t g_peak_usage = 0;
static size_t g_current_usage = 0;

void* AllocateAligned(size_t size, size_t alignment) {
    if (size == 0) {
        return nullptr;
    }
    
#ifdef _WIN32
    void* ptr = _aligned_malloc(size, alignment);
#else
    void* ptr = aligned_alloc(alignment, size);
#endif
    
    if (ptr) {
        g_total_allocated += size;
        g_current_usage += size;
        if (g_current_usage > g_peak_usage) {
            g_peak_usage = g_current_usage;
        }
    }
    
    return ptr;
}

void FreeAligned(void* ptr) {
    if (!ptr) {
        return;
    }
    
    // Note: We can't track the exact size freed without additional bookkeeping
    // This is a simplified implementation
    g_total_freed += 1; // Simplified tracking
    g_current_usage = (g_current_usage > 1) ? g_current_usage - 1 : 0;
    
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void* AllocateArray(size_t count, size_t element_size) {
    if (count == 0 || element_size == 0) {
        return nullptr;
    }
    
    size_t total_size = count * element_size;
    
    // Check for overflow
    if (total_size / element_size != count) {
        return nullptr;
    }
    
    void* ptr = malloc(total_size);
    if (ptr) {
        g_total_allocated += total_size;
        g_current_usage += total_size;
        if (g_current_usage > g_peak_usage) {
            g_peak_usage = g_current_usage;
        }
    }
    
    return ptr;
}

void FreeArray(void* ptr) {
    if (!ptr) {
        return;
    }
    
    // Note: We can't track the exact size freed without additional bookkeeping
    // This is a simplified implementation
    g_total_freed += 1; // Simplified tracking
    g_current_usage = (g_current_usage > 1) ? g_current_usage - 1 : 0;
    
    free(ptr);
}

bool SafeMemcpy(void* dest, const void* src, size_t size) {
    if (!dest || !src || size == 0) {
        return false;
    }
    
    // Check for overlap
    const uint8_t* src_ptr = static_cast<const uint8_t*>(src);
    uint8_t* dest_ptr = static_cast<uint8_t*>(dest);
    
    if (dest_ptr >= src_ptr && dest_ptr < src_ptr + size) {
        // Overlapping memory, use memmove instead
        memmove(dest, src, size);
    } else {
        memcpy(dest, src, size);
    }
    
    return true;
}

bool SafeMemset(void* dest, int value, size_t size) {
    if (!dest || size == 0) {
        return false;
    }
    
    memset(dest, value, size);
    return true;
}

int SafeMemcmp(const void* ptr1, const void* ptr2, size_t size) {
    if (!ptr1 || !ptr2 || size == 0) {
        return 0;
    }
    
    return memcmp(ptr1, ptr2, size);
}

void GetMemoryStats(size_t& total_allocated, size_t& total_freed, size_t& peak_usage) {
    total_allocated = g_total_allocated;
    total_freed = g_total_freed;
    peak_usage = g_peak_usage;
}

void ResetMemoryStats() {
    g_total_allocated = 0;
    g_total_freed = 0;
    g_peak_usage = 0;
    g_current_usage = 0;
}

bool CheckMemoryLeaks() {
    // This is a simplified check - in a real implementation,
    // you would need more sophisticated tracking
    return g_current_usage > 0;
}

} // namespace Utils
} // namespace PDFLib 