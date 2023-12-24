/**************************************************************************/
/*              COPYRIGHT Carnegie Mellon University 2023                 */
/* Do not post this file or any derivative on a public site or repository */
/**************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "lib/xalloc.h"
#include "lib/stack.h"
#include "lib/contracts.h"
#include "lib/c0v_stack.h"
#include "lib/c0vm.h"
#include "lib/c0vm_c0ffi.h"
#include "lib/c0vm_abort.h"

/* call stack frames */
typedef struct frame_info frame;
struct frame_info
{
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Function body */
  size_t pc;     /* Program counter */
  c0_value *V;   /* The local variables */
};

int32_t pop_int(c0v_stack_t S)
{
  REQUIRES(S != NULL);
  return val2int(c0v_pop(S));
}

void push_int(c0v_stack_t S, int32_t i)
{
  REQUIRES(S != NULL);
  c0v_push(S, int2val(i));
}

void *pop_ptr(c0v_stack_t S)
{
  REQUIRES(S != NULL);
  return val2ptr(c0v_pop(S));
}

void push_ptr(c0v_stack_t S, void *p)
{
  REQUIRES(S != NULL);
  c0v_push(S, ptr2val(p));
}

c0_tagged_ptr *pop_taggedptr(c0v_stack_t S)
{
  REQUIRES(S != NULL);
  return val2tagged_ptr(c0v_pop(S));
}

void push_taggedptr(c0v_stack_t S, void *p, uint16_t tag)
{
  REQUIRES(S != NULL);
  c0v_push(S, tagged_ptr2val(p, tag));
}

int execute(struct bc0_file *bc0)
{
  REQUIRES(bc0 != NULL);

  /* Variables */
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Array of bytes that make up the current function */
  size_t pc;     /* Current location within the current byte array P */
  c0_value *V;   /* Local variables (you won't need this till Task 2) */
  (void)V;       // silences compilation errors about V being currently unused

  S = c0v_stack_new();
  P = bc0->function_pool[0].code; // Initialized to main
  pc = 0;
  V = xcalloc(bc0->function_pool[0].num_vars, sizeof(c0_value)); // Free later

  /* The call stack, a generic stack that should contain pointers to frames */
  /* You won't need this until you implement functions. */
  gstack_t callStack;
  callStack = stack_new(); // silences compilation errors about callStack being currently unused

  while (true)
  {

#ifdef DEBUG
    /* You can add extra debugging information here */
    fprintf(stderr, "Opcode %x -- Stack size: %zu -- PC: %zu\n",
            P[pc], c0v_stack_size(S), pc);
#endif

    switch (P[pc])
    {

      /* Additional stack operation: */

    case POP:
    {
      pc++;
      c0v_pop(S);
      break;
    }

    case DUP:
    {
      pc++;
      c0_value v = c0v_pop(S);
      c0v_push(S, v);
      c0v_push(S, v);
      break;
    }

    case SWAP:
    {
      pc++;
      c0_value v2 = c0v_pop(S);
      c0_value v1 = c0v_pop(S);
      c0v_push(S, v2);
      c0v_push(S, v1);
      break;
    }

      /* Returning from a function.
       * This currently has a memory leak! You will need to make a slight
       * change for the initial tasks to avoid leaking memory.  You will
       * need to revise it further when you write INVOKESTATIC. */

    case RETURN:
    {
      c0_value retval = c0v_pop(S);
      ASSERT(c0v_stack_empty(S));
      // Another way to print only in DEBUG mode
      // IF_DEBUG(fprintf(stderr, "Returning %d from execute()\n", val2int(retval)));
      // Free everything before returning from the execute function!
      free(V);
      c0v_stack_free(S);
      if (stack_empty(callStack))
      {
        stack_free(callStack, NULL);
        return val2int(retval);
      }
      else
      {
        frame *curr_frame = pop(callStack);
        S = curr_frame->S;
        P = curr_frame->P;
        V = curr_frame->V;
        pc = curr_frame->pc;
        free(curr_frame);
        c0v_push(S, retval);
        break;
      }
    }

      /* Arithmetic and Logical operations */

    case IADD:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      int32_t res = x + y;
      push_int(S, res);
      break;
    }

    case ISUB:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      int32_t res = x - y;
      push_int(S, res);
      break;
    }

    case IMUL:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      int32_t res = x * y;
      push_int(S, res);
      break;
    }

    case IDIV:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      if (y == 0)
      {
        c0_arith_error("Div by 0 !\n");
      }
      else if (y == -1 && x == INT32_MIN)
      {
        c0_arith_error("Error: INT_MIN / -1 !\n");
      }
      else
      {
        int32_t res = x / y;
        push_int(S, res);
      }
      break;
    }

    case IREM:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      if (y == 0)
      {
        c0_arith_error("% by 0 !\n");
      }
      else if (y == -1 && x == INT32_MIN)
      {
        c0_arith_error("Error: INT_MIN % -1 !\n");
      }
      else
      {
        int32_t res = x % y;
        push_int(S, res);
      }
      break;
    }

    case IAND:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      int32_t res = x & y;
      push_int(S, res);
      break;
    }

    case IOR:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      int32_t res = x | y;
      push_int(S, res);
      break;
    }

    case IXOR:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      int32_t res = x ^ y;
      push_int(S, res);
      break;
    }

    case ISHR:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      if (y < 0)
      {
        c0_arith_error("Error: shift < 0 !\n");
      }
      else if (y >= 32)
      {
        c0_arith_error("Error: shift >= 32 !\n");
      }
      else
      {
        int32_t res = x >> y;
        push_int(S, res);
      }

      break;
    }

    case ISHL:
    {
      pc++;
      int32_t y = pop_int(S);
      int32_t x = pop_int(S);
      if (y < 0)
      {
        c0_arith_error("Error: shift < 0 !\n");
      }
      else if (y >= 32)
      {
        c0_arith_error("Error: shift >= 32 !\n");
      }
      else
      {
        int32_t res = x << y;
        push_int(S, res);
      }

      break;
    }

      /* Pushing constants */

    case BIPUSH:
    {
      pc++;
      int32_t x = (int32_t)(int8_t)P[pc];
      push_int(S, x);
      pc++;
      break;
    }

    case ILDC:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t idx = (c1 << 8) | c2;
      if (idx > bc0->int_count)
        c0_arith_error("Invalid Index!");
      int32_t x = bc0->int_pool[idx];
      push_int(S, x);
      pc++;
      break;
    }

    case ALDC:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t idx = (c1 << 8) | c2;
      if (idx > bc0->string_count)
        c0_arith_error("Invalid Index!");
      char *x = &bc0->string_pool[idx];
      push_ptr(S, (void *)x);
      pc++;
      break;
    }

    case ACONST_NULL:
    {
      pc++;
      push_ptr(S, NULL);
      break;
    }

      /* Operations on local variables */

    case VLOAD:
    {
      pc++;
      c0_value v = V[P[pc]];
      c0v_push(S, v);
      pc++;
      break;
    }

    case VSTORE:
    {
      pc++;
      V[P[pc]] = c0v_pop(S);
      pc++;
      break;
    }

      /* Assertions and errors */

    case ATHROW:
    {
      pc++;
      void *a = pop_ptr(S);
      c0_user_error((char *)a);
      break;
    }

    case ASSERT:
    {
      pc++;
      void *a = pop_ptr(S);
      int32_t x = pop_int(S);
      if (x == 0)
        c0_assertion_failure((char *)a);
      break;
    }

      /* Control flow operations */

    case NOP:
    {
      pc++;
      break;
    }

    case IF_CMPEQ:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      c0_value v2 = c0v_pop(S);
      c0_value v1 = c0v_pop(S);

      if (val_equal(v1, v2))
        pc += ((int16_t)((o1 << 8) | o2)) - 2;
      else
        pc++;

      break;
    }

    case IF_CMPNE:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      c0_value v2 = c0v_pop(S);
      c0_value v1 = c0v_pop(S);

      if (!val_equal(v1, v2))
        pc += ((int16_t)((o1 << 8) | o2)) - 2;
      else
        pc++;

      break;
    }

    case IF_ICMPLT:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      int32_t y = pop_int(S);
      int32_t x = pop_int(S);

      if (x < y)
        pc += ((int16_t)((o1 << 8) | o2)) - 2;
      else
        pc++;

      break;
    }

    case IF_ICMPGE:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      int32_t y = pop_int(S);
      int32_t x = pop_int(S);

      if (x >= y)
        pc += ((int16_t)((o1 << 8) | o2)) - 2;
      else
        pc++;

      break;
    }

    case IF_ICMPGT:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      int32_t y = pop_int(S);
      int32_t x = pop_int(S);

      if (x > y)
        pc += ((int16_t)((o1 << 8) | o2)) - 2;
      else
        pc++;

      break;
    }

    case IF_ICMPLE:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      int32_t y = pop_int(S);
      int32_t x = pop_int(S);

      if (x <= y)
        pc += ((int16_t)((o1 << 8) | o2)) - 2;
      else
        pc++;

      break;
    }

    case GOTO:
    {
      pc++;
      uint16_t o1 = (uint16_t)P[pc];
      pc++;
      uint16_t o2 = (uint16_t)P[pc];

      pc += ((int16_t)((o1 << 8) | o2)) - 2;

      break;
    }

      /* Function call operations: */

    case INVOKESTATIC:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t idx = (c1 << 8) | c2;
      assert(idx < bc0->function_count);
      pc++;

      frame *caller = xcalloc(1, sizeof(frame));
      caller->S = S;
      caller->P = P;
      caller->pc = pc;
      caller->V = V;

      push(callStack, (stack_elem)caller);

      struct function_info *called = &bc0->function_pool[idx];

      V = xcalloc(called->num_vars, sizeof(c0_value));
      for (uint8_t i = 0; i < called->num_args; i++)
      {
        V[called->num_args - 1 - i] = c0v_pop(S);
      }

      pc = 0;
      S = c0v_stack_new();
      P = called->code;

      break;
    }

    case INVOKENATIVE:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t idx = (c1 << 8) | c2;
      assert(idx < bc0->native_count);
      pc++;

      struct native_info *native = &bc0->native_pool[idx];

      c0_value *TMP = xcalloc(native->num_args, sizeof(c0_value));
      for (uint8_t i = 0; i < native->num_args; i++)
      {
        TMP[native->num_args - 1 - i] = c0v_pop(S);
      }

      c0_value return_val = (*native_function_table[native->function_table_index])(TMP);
      c0v_push(S, return_val);
      free(TMP);
      break;
    }

      /* Memory allocation and access operations: */

    case NEW:
    {
      pc++;
      size_t s = (size_t)(uint8_t)P[pc];
      void *a = xcalloc(1, s); // Free? No.
      push_ptr(S, a);
      pc++;
      break;
    }

    case IMLOAD:
    {
      void *a = pop_ptr(S);
      if (a == NULL)
        c0_memory_error(" NULL ptr read!\n");
      int32_t x = *((int32_t *)a);
      push_int(S, x);
      pc++;
      break;
    }

    case IMSTORE:
    {
      int32_t x = pop_int(S);
      void *a = pop_ptr(S);
      if (a == NULL)
        c0_memory_error("NULL ptr write!\n");
      *((int32_t *)a) = x;
      pc++;
      break;
    }

    case AMLOAD:
    {
      void **a = pop_ptr(S);
      if (a == NULL)
        c0_memory_error(" NULL ptr read!\n");
      void *b = *a;
      push_ptr(S, b);
      pc++;
      break;
    }

    case AMSTORE:
    {
      void **b = pop_ptr(S);
      void **a = pop_ptr(S);
      if (a == NULL)
        c0_memory_error(" NULL ptr read!\n");
      *a = b;
      pc++;
      break;
    }

    case CMLOAD:
    {
      char *a = (char *)pop_ptr(S);
      if (a == NULL)
        c0_memory_error(" NULL ptr read!\n");
      int32_t x = (int32_t)*a;
      push_int(S, x);
      pc++;
      break;
    }

    case CMSTORE:
    {
      int32_t x = pop_int(S);
      char *a = (char *)pop_ptr(S);
      if (a == NULL)
        c0_memory_error("NULL ptr write!\n");
      *a = x & 0x7F;
      pc++;
      break;
    }

    case AADDF:
    {
      pc++;
      ubyte f = (ubyte)P[pc];
      char *a = (char *)pop_ptr(S);
      if (a == NULL)
        c0_memory_error("NULL ptr offset!\n");
      push_ptr(S, (void *)(a + f));
      pc++;
      break;
    }

      /* Array operations: */

    case NEWARRAY:
    {
      pc++;
      uint32_t s = (uint32_t)P[pc];
      int32_t n = pop_int(S);
      if (n < 0)
        c0_memory_error("Negative size for array!\n");
      c0_array *a = xmalloc(sizeof(c0_array));
      a->count = (uint32_t)n;
      a->elt_size = s;
      a->elems = xcalloc((uint32_t)n, s);
      push_ptr(S, a);
      pc++;
      break;
    }

    case ARRAYLENGTH:
    {
      c0_array *a = (c0_array *)pop_ptr(S);
      if (a == NULL)
        c0_memory_error("NULL ptr length!\n");
      int32_t n = (int32_t)a->count;
      push_int(S, n);
      pc++;
      break;
    }

    case AADDS:
    {
      int32_t i = pop_int(S);
      c0_array *a = (c0_array *)pop_ptr(S);
      uint32_t offset = ((uint32_t)i) * a->elt_size;
      if ((i < 0) || (uint32_t)i >= a->count)
        c0_memory_error("Not a valid index!\n");
      push_ptr(S, ((char *)a->elems) + offset);
      pc++;
      break;
    }

      /* BONUS -- C1 operations */

    case CHECKTAG:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t tag = (c1 << 8) | c2;
      c0_tagged_ptr *a_tagged = pop_taggedptr(S);
      if (a_tagged != NULL && a_tagged->tag != tag)
        c0_memory_error("Invalid Tagged Pointer!\n");
      if (a_tagged == NULL)
      {
        push_ptr(S, (void *)a_tagged);
      }
      else
      {
        push_ptr(S, a_tagged->p);
      }

      pc++;
      break;
    }

    case HASTAG:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t tag = (c1 << 8) | c2;
      c0_tagged_ptr *a_tagged = pop_taggedptr(S);
      int32_t x = (a_tagged != NULL && a_tagged->tag != tag) ? 0 : 1;
      push_int(S, x);
      pc++;
      break;
    }

    case ADDTAG:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t tag = (c1 << 8) | c2;
      void *a = pop_ptr(S); // Assuming Untagged
      push_taggedptr(S, a, tag);
      pc++;
      break;
    }

    case ADDROF_STATIC:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t idx = (c1 << 8) | c2;
      void *a = create_funptr(false, idx);
      push_ptr(S, a);
      pc++;
      break;
    }

    case ADDROF_NATIVE:
    {
      pc++;
      uint16_t c1 = (uint16_t)P[pc];
      pc++;
      uint16_t c2 = (uint16_t)P[pc];
      uint16_t idx = (c1 << 8) | c2;
      void *a = create_funptr(true, idx);
      push_ptr(S, a);
      pc++;
      break;
    }

    case INVOKEDYNAMIC:
    {
      void *a = pop_ptr(S);
      ASSERT(is_funptr(a));
      uint16_t idx = funptr2index(a);

      if (is_native_funptr(a))
      {
        pc++;
        struct native_info *native = &bc0->native_pool[idx];

        c0_value *TMP = xcalloc(native->num_args, sizeof(c0_value));
        for (uint8_t i = 0; i < native->num_args; i++)
        {
          TMP[native->num_args - 1 - i] = c0v_pop(S);
        }

        c0_value return_val = (*native_function_table[native->function_table_index])(TMP);
        c0v_push(S, return_val);
        free(TMP);
        break;
      }
      else
      {
        pc++;
        frame *caller = xcalloc(1, sizeof(frame));
        caller->S = S;
        caller->P = P;
        caller->pc = pc;
        caller->V = V;

        push(callStack, (stack_elem)caller);

        struct function_info *called = &bc0->function_pool[idx];

        V = xcalloc(called->num_vars, sizeof(c0_value));
        for (uint8_t i = 0; i < called->num_args; i++)
        {
          V[called->num_args - 1 - i] = c0v_pop(S);
        }

        pc = 0;
        S = c0v_stack_new();
        P = called->code;
        break;
      }
    }

    default:
      fprintf(stderr, "invalid opcode: 0x%02x\n", P[pc]);
      abort();
    }
  }

  /* cannot get here from infinite loop */
  assert(false);
}
