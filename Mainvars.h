extern int skip_steps;


// Integration parameters
extern int Duration;
extern const double Transient;
extern const double StepSize;
extern const int N_curvs;

// Fitness traj
extern const double    AvgSpeed;              // Average speed of the worm in meters per seconds
extern const double    BBCfit;

// Genotype -> Phenotype Mapping Ranges
extern const double    BiasRange ;
extern const double    SCRange ;    
extern const double    CSRange ; 
extern const double    ESRange  ;
extern const double    SRmax  ;  
extern const double    NMJmax  ; 
extern const double    NMJmin   ;

extern const int SR_A;
extern const int SR_B;

// Size of genotype
extern int	VectSize;
extern string output_dir_name;
//extern int nn(int, int);
extern string nervousSystemName;