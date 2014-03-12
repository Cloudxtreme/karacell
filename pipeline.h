#if !(defined(PIPELINE)||defined(PIPELINE_OFF))
  #error "Use 'gcc -DPIPELINE' to enable IO pipelining or 'gcc -DPIPELINE_OFF' not to."
#elif defined(PIPELINE)&&defined(PIPELINE_OFF)
  #error "You have defined both PIPELINE and PIPELINE_OFF. Chose one only."
#endif
