/**
 * @file memory_utils.h
 * @brief Memory utility functions for PDF_LIB
 */

#ifndef PDF_LIB_MEMORY_UTILS_H
#define PDF_LIB_MEMORY_UTILS_H

#include <cstddef>
#include <vector>

namespace PDFLib {
namespace Utils {

/**
 * @brief Allocate memory with alignment
 * @param size Size in bytes
 * @param alignment Alignment requirement
 * @return Pointer to allocated memory or nullptr on failure
 */
void* AllocateAligned(size_t size, size_t alignment = 16);

/**
 * @brief Free aligned memory
 * @param ptr Pointer to memory to free
 */
void FreeAligned(void* ptr);

/**
 * @brief Allocate memory for array
 * @param count Number of elements
 * @param element_size Size of each element
 * @return Pointer to allocated memory or nullptr on failure
 */
void* AllocateArray(size_t count, size_t element_size);

/**
 * @brief Free array memory
 * @param ptr Pointer to memory to free
 */
void FreeArray(void* ptr);

/**
 * @brief Copy memory with bounds checking
 * @param dest Destination pointer
 * @param src Source pointer
 * @param size Number of bytes to copy
 * @return true if copy successful, false otherwise
 */
bool SafeMemcpy(void* dest, const void* src, size_t size);

/**
 * @brief Set memory to value with bounds checking
 * @param dest Destination pointer
 * @param value Value to set
 * @param size Number of bytes to set
 * @return true if operation successful, false otherwise
 */
bool SafeMemset(void* dest, int value, size_t size);

/**
 * @brief Compare memory with bounds checking
 * @param ptr1 First pointer
 * @param ptr2 Second pointer
 * @param size Number of bytes to compare
 * @return 0 if equal, negative if ptr1 < ptr2, positive if ptr1 > ptr2
 */
int SafeMemcmp(const void* ptr1, const void* ptr2, size_t size);

/**
 * @brief Get memory usage statistics
 * @param total_allocated Output total allocated bytes
 * @param total_freed Output total freed bytes
 * @param peak_usage Output peak memory usage
 */
void GetMemoryStats(size_t& total_allocated, size_t& total_freed, size_t& peak_usage);

/**
 * @brief Reset memory statistics
 */
void ResetMemoryStats();

/**
 * @brief Check for memory leaks
 * @return true if memory leaks detected, false otherwise
 */
bool CheckMemoryLeaks();

} // namespace Utils
} // namespace PDFLib

#endif // PDF_LIB_MEMORY_UTILS_H 