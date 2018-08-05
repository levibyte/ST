/* This file is part of the "slavicTales" project.
 * You may use, distribute or modify this code under the terms
 * of the GNU General Public License version 2.
 * See LICENCE.txt in the root directory of the project.
 *
 * Author: Maxim Atanasov
 * E-mail: atanasovmaksim1@gmail.com
 */

#ifndef SLAVICTALES_TASK_ALLOCATOR_HPP
#define SLAVICTALES_TASK_ALLOCATOR_HPP

#define TASK_ALLOCATOR_CAPACITY 20

#include <defs.hpp>
#include <task_manager/task.hpp>

///An allocator for task objects
class task_allocator {
private:
    SDL_mutex* access_mutex{};
    uint16_t pointer = 0;
    ST::task* memory{};
    uint32_t memory_size = TASK_ALLOCATOR_CAPACITY;
    bool allocated[TASK_ALLOCATOR_CAPACITY]{};
public:
    ST::task* allocate_task(void (*function)(void *), void *arg, SDL_semaphore *dependency, int affinity);
    void deallocate(uint16_t id);
    task_allocator();
    ~task_allocator();
};

/**
 * //only use these functions to create/destroy tasks
 * @param function A function representing a work task
 * @param arg The arguments to above function
 * @param dependency A lock acting as a dependency to the task or nullptr if there is no such dependency
 * @param affinity Thread affinity for the task
 * @return A new task object
 */
ST::task* make_task(void (*function)(void *), void *arg, SDL_semaphore *dependency, int affinity);

/**
 * Destroys a task using the task allocator
 * @param task The task to destroy
 */
void destroy_task(ST::task* task);

#endif //SLAVICTALES_TASK_ALLOCATOR_HPP