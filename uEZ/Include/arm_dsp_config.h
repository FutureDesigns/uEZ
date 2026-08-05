/* arm_dsp_config.h for project specific settings
 *
 */
#ifndef ARM_DSP_CONFIG_H_
#define ARM_DSP_CONFIG_H_

// Whenever #include <arm_math.h> is used in a project, set these defines in the line above it to customize the build mode, or define the ARM_DSP_CUSTOM_CONFIG macro in the project itself.
// #define ARM_DSP_CUSTOM_CONFIG 	   // When set, the file arm_dsp_config.h is included by the arm_math_types.h headers.

// Common Preprocessor Macros to set for DSP files

//#define ARM_MATH_LOOPUNROLL          // Define macro ARM_MATH_LOOPUNROLL to enable manual loop unrolling in DSP functions
//#define ARM_MATH_MATRIX_CHECK        // Define macro ARM_MATH_MATRIX_CHECK for checking on the input and output sizes of matrices
//#define ARM_MATH_ROUNDING            // Define macro ARM_MATH_ROUNDING for rounding on support functions
//#define DISABLEFLOAT16               // Disable float16 algorithms when __fp16 is not supported for a specific compiler / core configuration. This is only valid for scalar. When vector architecture is supporting f16 then it can't be disabled.

//#define ARM_DSP_ATTRIBUTE            // Can be set to define CMSIS-DSP function as weak functions. This can either be set on the command line when building or in a new arm_dsp_config.h header (see below)
//#define ARM_DSP_TABLE_ATTRIBUTE      // Can be set to define in which section constant tables must be mapped. This can either be set on the command line when building or in a new arm_dsp_config.h header (see below). Another way to set those sections is by modifying the linker scripts since the constant tables are defined only in a restricted set of source files.
//#define ARM_DSP_TABLE_ATTRIBUTE      // ".const_dsp_tabls" // place the constant read only number arrays in a particular read only section

// Neon/Helium/MVE common settings
//#define ARM_MATH_AUTOVECTORIZE       // With Helium or Neon, disable the use of vectorized code with C intrinsics and use pure C instead. The vectorization is then done by the compiler. Check the targeted functions to see which version vectored "better".

// Neon only settings
//#define ARM_MATH_NEON                // Define macro ARM_MATH_NEON to enable Neon versions of the DSP functions. It is not enabled by default when Neon is available because performances are dependent on the compiler and target architecture.
//#define ARM_MATH_NEON_EXPERIMENTAL   // Define macro ARM_MATH_NEON_EXPERIMENTAL to enable experimental Neon versions of of some DSP functions. Experimental Neon versions currently do not have better performances than the scalar versions.

// Helium/MVE only settings
//#define ARM_MATH_HELIUM              // It implies the flags ARM_MATH_MVEF and ARM_MATH_MVEI and ARM_MATH_MVE_FLOAT16.
//#define ARM_MATH_HELIUM_EXPERIMENTAL // Only taken into account when ARM_MATH_MVEF, ARM_MATH_MVEI or ARM_MATH_MVE_FLOAT16 are defined. Enable some vector versions which may have worse performance than scalar depending on the core / compiler configuration.
//#define ARM_MATH_MVEF                // Select Helium versions of the f32 algorithms. It implies ARM_MATH_FLOAT16 and ARM_MATH_MVEI.
//#define ARM_MATH_MVEI                // Select Helium versions of the int and fixed point algorithms.
//#define ARM_MATH_MVE_FLOAT16         // MVE Float16 implementations of some algorithms (Requires MVE extension).

#endif /* ARM_DSP_CONFIG_H_ */
