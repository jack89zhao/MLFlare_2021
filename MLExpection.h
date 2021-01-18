//
//  MLExpection.h
//  MLFlare
//
//  Created by Jackie Wang on 2019/7/6.
//  Copyright © 2019 Jackie Wang. All rights reserved.
//

#ifndef MLExpection_h
#define MLExpection_h

#include <stdio.h>
#include <setjmp.h>

jmp_buf JumpBuffer;
#define try if (!setjmp(JumpBuffer))
#define catch else
#define throw  longjmp(JumpBuffer, 1)

#endif /* MLExpection_h */
