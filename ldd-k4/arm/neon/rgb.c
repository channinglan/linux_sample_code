#include <stdint.h>
#include <stdio.h>
#include <arm_neon.h>	//need to include this ifyou want to use intrinsics

void reference_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int n)
{
  int i;
  for (i=0; i<n; i++)
  {
    int r = *src++; // load red
    int g = *src++; // load green
    int b = *src++; // load blue 

    // build weighted average:
    int y = (r*77)+(g*151)+(b*28);

    // undo the scale by 256 and write to memory:
    *dest++ = (y>>8);
  }
}

void neon_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int n)
{
  int i;
  /*
  First off I load my weight factors into three NEON registers. The vdup.8 instruction does this
  and also replicates the byte into all 8 bytes of the NEON register.
  */
  uint8x8_t rfac = vdup_n_u8 (77);
  uint8x8_t gfac = vdup_n_u8 (151);
  uint8x8_t bfac = vdup_n_u8 (28);
  n/=8;

  for (i=0; i<n; i++)
  {
    uint16x8_t  temp;

    /*Now I load 8 pixels at once into three registers*/
    uint8x8x3_t rgb  = vld3_u8 (src);
    uint8x8_t result;

    /*vmull.u8 multiplies each byte of the first argument with each corresponding byte of the second argument. */
    temp = vmull_u8 (rgb.val[0],      rfac);
    /*vmlal.u8 does the same thing as vmull.u8 but also adds the content of another register to the result.*/
    temp = vmlal_u8 (temp,rgb.val[1], gfac);
    temp = vmlal_u8 (temp,rgb.val[2], bfac);

    result = vshrn_n_u16 (temp, 8);
    /* store the result*/
    vst1_u8 (dest, result);
    src  += 8*3;
    dest += 8;
  }
}

main()
{
	uint8_t src[100];
	uint8_t dst[100];
	reference_convert(dst, src, 8);
	printf("%d %d %d %d\n", dst[0], dst[1], dst[2], dst[3]);
	neon_convert(dst, src, 8);
	printf("%d %d %d %d\n", dst[0], dst[1], dst[2], dst[3]);
}
