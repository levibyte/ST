/* This file is part of the "ST" project.
 * You may use, distribute or modify this code under the terms
 * of the GNU General Public License version 2.
 * See LICENCE.txt in the root directory of the project.
 *
 * Author: Maxim Atanasov
 * E-mail: maxim.atanasov@protonmail.com
 */

#include <cstdlib>

#ifdef __DEBUG

void* operator new(size_t size){
    return malloc(size);
}

void operator delete(void* ptr){
    free(ptr);
}

void* operator new[](size_t size){
    return malloc(size);
}

void operator delete[](void* ptr){
    free(ptr);
}

#endif