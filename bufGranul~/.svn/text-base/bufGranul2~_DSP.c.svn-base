
/************************************************************************
 *
 *			                >>>>>>> BUFGRANUL2~ <<<<<<<
 *
 *						  multi-buffer enveloppe externe, 
 *                          multi-buffer son externe.
 *              continuite des grains lors d'un changement de buffer.
 *                         controle en float et/ou en audio
 * 					selection de buffer son par entree signal
 *                           ----------------------
 *                              GMEM 2002-2004
 *         Laurent Pottier / Loic Kessous / Charles Bascou / Leopold Frey
 *
 * -----------------------------------------------------------------------
 *
 * 
 * N.B. 
 *      * Pour compatibilite maximum Mac/PC :
 *        pas d'accents dans les commentaires svp
 *
 *      * Faites des commentaires !
 *
 ************************************************************************/
 
 /*** DSP implementation file ***/ 
 
#include "bufGranul2~.h"

t_int *bufGranul_perform1(t_int *w)
{   
	t_bufGranul *x = (t_bufGranul *)(w[1]);

    t_float * in			= (float *)(w[2]);
	t_float * t_begin 		= (float *)(w[3]);
    t_float * t_transpos 	= (float *)(w[4]);
    t_float * t_amp			= (float *)(w[5]);
    t_float * t_length 		= (float *)(w[6]);
    t_float * t_pan 		= (float *)(w[7]);
    t_float * t_dist 		= (float *)(w[8]);
    t_float * t_sndbuf 		= (float *)(w[9]);

    t_float *out1 = (t_float *)(w[10]);
      
    int n = (int)(w[11]);

	int t_buf;
	
    int N = n;
    int xn = 0, p;

	double srms = x->x_sr*0.001; // double for precision
	double indT;
	float val;
    float sigin, prevsigin,savein; // values of actual and previous input sample   

	double beginind = x->x_begin*srms; // double for precision (this number can be very large)
	float lengthind = x->x_length*srms;	
	
	// temporary buffer index variable
	int t_active_buf ; // buffer actif pour la crŽation si pas entrŽe bufnum signal ( x->x_active_buf peut ne plus tre valide )
	int t_bufnum;
	int t_bufchan;
	int t_active_env;
	int t_envnum;
	float * t_bufsamples;
	float * t_envsamples;

    int i,j;
	int nvoices = x->x_nvoices;
	int loop = x->x_loop;
	
	int initTest;
	
	// variables grains
	int t_bufframes;
	
	float finc;
	double temp_sind;
	double source_findex, target_findex;
	int dsp_i_begin;
	int dsp_i_end;
		
	long loop_length;
	long loop_start;
	
	int nech_process, nech_process_sat;
	
		//trick interp long index
	long base_lindex;
	long lindex;
	long lincr;
	
	long base_lindex_env;
	long lindex_env;
	long lincr_env;
	
	int buffer_index;
	int buffer_index_plus_1;
	int interp_table_index;
	
	t_linear_interp * interp_table = x->x_linear_interp_table;

    
	// debug var
#ifdef PERF_DEBUG
    int n_voices_active;
#endif

   	sigin = x->x_sigin;
   	
   	
#ifdef PERF_DEBUG
	if(x->ask_poll)
	{
		n_voices_active=0;
	
		for(i=0; i< nvoices; i++) if (x->x_voiceOn[i] ) n_voices_active++;
		
		SETSYM( x->info_list, gensym("voices"));
		SETLONG(x->info_list+1, n_voices_active); 
		outlet_list(x->info,0l,2,x->info_list);
	}
#endif

	initTest = bufGranul_bufferinfos(x);
   	if(initTest == 0) goto zero;
	else
	{
		t_active_buf = buffer_check(x,x->x_active_buf);
		t_active_env = bufferenv_check(x,x->x_active_env);
		// %%%%%%%%%%     creation de grains   %%%%%%%%%%%%%%%%%
   			
   		p=nvoices;
   		
		// Dans le cas d'un grain declenche par un bang
		savein = in[0];
   		if(x->x_askfor)
   		{
			sigin = -1.;
			in[0] = 1.;
			x->x_askfor = 0;
   		}

		// Meme chose avec declenchement par zero-crossing sur tout le vecteur in, dans la limite fixee par nvoices
		while (n-- && p)
		{
			//-----signal zero-crossing ascendant declenche un grain----//
			prevsigin = sigin;
			sigin = in[xn];
			
			if (prevsigin <= 0 && sigin > 0) // creation d'un grain
			{
				while(--p && x->x_voiceOn[p] ) { }  // avance jusqu a la 1ere voix libre
				
				if(p)
				{
    				x->x_voiceOn[p] = 1;
					x->x_sind[p] = x->Vbeg[p] = (x->x_in2con) * t_begin[xn]*srms + (1 - x->x_in2con)* beginind;	// index dans buffer
					x->Vtranspos[p] = (x->x_in3con) * t_transpos[xn] + (1 - x->x_in3con) * x->x_transpos;			// valeur de pitch
					x->Vamp[p]		= (x->x_in4con) * t_amp[xn] + (1 - x->x_in4con) * x->x_amp;						// amplitude
					x->Vbuf[p] = (x->x_in8con) ? buffer_check(x,(int) t_sndbuf[xn] ) :  t_active_buf;		// numero du buffer son	
					x->Venv[p] = t_active_env;	// enveloppe active pour ce grain
     
					if(x->x_in5con)
					{
						if(t_length[xn]<0)
						{	
							x->Vlength[p]	= -t_length[xn]*srms;
							x->envinc[p]	= -1.*(float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= x->x_env_frames[t_active_env] - 1;
						}
						else
						{
							x->Vlength[p]	= t_length[xn]*srms;
							x->envinc[p]	= (float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= 0. ;
						}
					}
					else
					{
						x->Vlength[p] = lengthind;
						x->envinc[p] = x->x_env_dir * x->x_env_frames[t_active_env] / x->Vlength[p] ;
						if(x->x_env_dir < 0)
							x->envind[p] =  x->x_env_frames[t_active_env] - 1;
						else 
							x->envind[p] = 0.;
					}
          
					x->x_ind[p] = 0;
					x->x_remain_ind[p] = (long) x->Vlength[p];
					x->x_delay[p] = xn;   // delay de declenchement dans le vecteur         	  
    					
				} else goto perform;
			}
			xn++ ; // boucle on incremente le pointeur dans le vecteur in et le delay de declenchement
		}
   		 
   		 
		// %%%%%%%%%%     fin creation     %%%%%%%%%%%%%%%%%
   		
   		perform :
		
		in[0] = savein;
   		//&&&&&&&&&&&&&  Boucle DSP  &&&&&&&&&&&&&
   		n = N;
   		j=0;
		while (n--)
		{
			out1[j] = 0;
			j++;   
		}	
    		 	         		   				 									 
		for (i=0; i < nvoices; i++)
		{         
			//si la voix est en cours              
			if (x->x_voiceOn[i]) //&& x->x_ind[i] < x->Vlength[i] )
			{
				// si delay + grand que taille vecteur on passe ˆ voix suivante 
				if(x->x_delay[i] >= N)
				{	
					x->x_delay[i] -= N ;
					goto next;
				}
				
				// nb d'ech a caluler pour ce vecteur
				nech_process = MIN( (N - x->x_delay[i]) , x->x_remain_ind[i] );
				
				// Selon le buffer dans lequel on doit prendre le son x->Vbuf[i] (voir creation des grains)
				// Il se peut que le buffer associŽ au grain ne soit plus valide -> verification -> si non on prend buffer #0
				t_bufnum = x->x_buf_valid_index[x->Vbuf[i]];
				t_bufframes =  x->x_buf_frames[t_bufnum];
				t_bufsamples = x->x_buf_samples[t_bufnum];
				t_bufchan = x->x_buf_nchan[t_bufnum];
				
				t_envsamples = x->x_env_samples[x->Venv[i]];
				
				// pas d'increment en float
				finc = (x->Vtranspos[i]*x->x_buf_sronsr[t_bufnum]);

				// snd index source 
				source_findex = x->x_sind[i];
				// snd index cible en fin de boucle
				target_findex = source_findex + nech_process * finc;
				
				
				// nb ech a calculer different selon boucle ou pas
				if( !loop )
				{
					// si index source hors des bornes -> kill grain
					if( source_findex >  x->x_buf_frames[t_bufnum] || source_findex <  0 )
					{
						x->x_voiceOn[i] = 0;
						goto next;
					}
					
					// nb SATURE d'ech a calculer pour ce vecteur (0 < ... < frames) ou (startloop < ... < endloop)
					if(target_findex >  t_bufframes )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule
						nech_process_sat = (long)(nech_process * (t_bufframes - source_findex) /  (target_findex - source_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
					
					}else if( target_findex < 0 )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule   ATTENTION arrondi depassement
						nech_process_sat = (long)(nech_process * (source_findex) /  (source_findex - target_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
						
					}
					else 
					{
						nech_process_sat = nech_process;
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
							
					}
				}
				else // if loop
				{
					/////////// LOOOP //////////
					nech_process_sat = nech_process;
					
					//target_findex = MOD(x->x_sind[i],x->x_buf_looplength[t_bufnum]);
					
					dsp_i_begin = x->x_delay[i];
					dsp_i_end = x->x_delay[i] + nech_process_sat;
					
					loop_start  = x->x_buf_loopstart[t_bufnum];
					loop_length = x->x_buf_looplength[t_bufnum];
					
					// calcul du vrai float index en ech de lecture
					temp_sind = x->x_sind[i];
					temp_sind -= loop_start;
			        temp_sind = loop_start + MOD(temp_sind,loop_length);  
					
					// calcul de l'index de lecture buffer en long
					base_lindex = (long) temp_sind;
					lindex = interp_index_scale( temp_sind - (double) base_lindex );
					lincr  = interp_index_scale(finc);					
					

				}
				
				
				// Enveloppe long index & incr calcul
				base_lindex_env = (long) x->envind[i];
				lindex_env = interp_index_scale( x->envind[i] - (double) base_lindex_env );
				lincr_env  = interp_index_scale(x->envinc[i]);
				
				x->envind[i] += nech_process_sat * x->envinc[i];
				
				
				
				//***********************************
				// CALCUL EFFECTIF DES ECHANTILLONS
				//***********************************
				
				if(loop)
					if( x->x_sinterp == 1 )  // loop + interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
							interp_table_index = interp_get_table_index( lindex_env );
			      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
			      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= loop_start;
							buffer_index = MODI(buffer_index,loop_length);
							
							buffer_index_plus_1 = buffer_index + 1;
							//buffer_index_plus_1 = MODI(buffer_index_plus_1,x->x_buf_looplength[t_bufnum]);
							
							interp_table_index = interp_get_table_index( lindex );
			      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
			      							+ interp_table[interp_table_index].b * t_bufsamples[buffer_index_plus_1 * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 
							
							out1[j] += val;
						}
					else	// loop + non interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
			      			x->x_env[i] = t_envsamples[buffer_index];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= x->x_buf_loopstart[t_bufnum];
							buffer_index = MODI(buffer_index,x->x_buf_looplength[t_bufnum]);
							
							val = t_bufsamples[buffer_index * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 
							
							out1[j] += val;

						}
					
				else // if non loop
				if( x->x_sinterp == 1 ) // non loop + interp 
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
						interp_table_index = interp_get_table_index( lindex_env );
		      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
		      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
						interp_table_index = interp_get_table_index( lindex );
		      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
		      							+ interp_table[interp_table_index].b * t_bufsamples[(buffer_index + 1) * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 
						
						out1[j] += val;

						
						
					}
				else		// non loop + non interp
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
		      			x->x_env[i] = t_envsamples[buffer_index];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
		      			val = t_bufsamples[buffer_index * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 
						
						out1[j] += val;
						
						
					}
			
				//post("%d %d dsp_i_begin",dsp_i_begin,dsp_i_end);
				
				//********************************
				//  MAJ de l'Žtat des grains
				
				x->x_sind[i] = target_findex;
				
				x->x_ind[i] += nech_process_sat;
				
				if( (x->x_remain_ind[i] -= nech_process_sat) <= 0)
				{	
					x->x_voiceOn[i] = 0;
					//post("remain kill");
				}
				
				// decremente delai
				x->x_delay[i] = MAX(x->x_delay[i] - N,0);
					
					

			} else { 
				
				//sinon = si la voix est libre// 
	   			x->x_voiceOn[i] = 0;		
			}
			 
			next:
			j=0; //DUMMY
			   
		} // fin for
			
        		  
		x->x_sigin = sigin;

		return (w+12);

	}

	zero:
		// Pas de declenchement de grains
		while (n--)
		{
    		*out1++ = 0.0;
		}
	
    out:    
		return (w+12);
}
//***********************************************************************************//
 
t_int *bufGranul_perform2(t_int *w)
{   
	t_bufGranul *x = (t_bufGranul *)(w[1]);

    t_float * in			= (float *)(w[2]);
	t_float * t_begin 		= (float *)(w[3]);
    t_float * t_transpos 	= (float *)(w[4]);
    t_float * t_amp			= (float *)(w[5]);
    t_float * t_length 		= (float *)(w[6]);
    t_float * t_pan 		= (float *)(w[7]);
    t_float * t_dist 		= (float *)(w[8]);
    t_float * t_sndbuf 		= (float *)(w[9]);

    t_float *out1 = (t_float *)(w[10]);
    t_float *out2 = (t_float *)(w[11]);
      
    int n = (int)(w[12]);

	int t_buf;
	
    int N = n;
    int xn = 0, p;

	double srms = x->x_sr*0.001; // double for precision
	double indT;
	float val;
	float val1,val2;
	float outsamp1,outsamp2;
    float sigin, prevsigin,savein; // values of actual and previous input sample   

	double beginind = x->x_begin*srms; // double for precision (this number can be very large)
	float lengthind = x->x_length*srms;	
	
	// temporary buffer index variable
	int t_active_buf ; // buffer actif pour la crŽation si pas entrŽe bufnum signal ( x->x_active_buf peut ne plus tre valide )
	int t_bufnum;
	int t_bufchan;
	int t_active_env;
	int t_envnum;
	float * t_bufsamples;
	float * t_envsamples;

    int i,j;
	int nvoices = x->x_nvoices;
	int loop = x->x_loop;
	
	int initTest;
	
	// variables grains
	int t_bufframes;
	
	float finc;
	double temp_sind;
	double source_findex, target_findex;
	int dsp_i_begin;
	int dsp_i_end;
		
	long loop_length;
	long loop_start;
	
	int nech_process, nech_process_sat;
	
		//trick interp long index
	long base_lindex;
	long lindex;
	long lincr;
	
	long base_lindex_env;
	long lindex_env;
	long lincr_env;
	
	int buffer_index;
	int buffer_index_plus_1;
	int interp_table_index;
	
	t_linear_interp * interp_table = x->x_linear_interp_table;

    
	// debug var
#ifdef PERF_DEBUG
    int n_voices_active;
#endif

   	sigin = x->x_sigin;
   	
   	
#ifdef PERF_DEBUG
	if(x->ask_poll)
	{
		n_voices_active=0;
	
		for(i=0; i< nvoices; i++) if (x->x_voiceOn[i] ) n_voices_active++;
		
		SETSYM( x->info_list, gensym("voices"));
		SETLONG(x->info_list+1, n_voices_active); 
		outlet_list(x->info,0l,2,x->info_list);
	}
#endif

	initTest = bufGranul_bufferinfos(x);
   	if(initTest == 0) goto zero;
	else
	{
		t_active_buf = buffer_check(x,x->x_active_buf);
		t_active_env = bufferenv_check(x,x->x_active_env);
		// %%%%%%%%%%     creation de grains   %%%%%%%%%%%%%%%%%
   			
   		p=nvoices;
   		
		// Dans le cas d'un grain declenche par un bang
		savein = in[0];
   		if(x->x_askfor)
   		{
			sigin = -1.;
			in[0] = 1.;
			x->x_askfor = 0;
   		}
		
		// Meme chose avec declenchement par zero-crossing sur tout le vecteur in, dans la limite fixee par nvoices
		while (n-- && p)
		{
			//-----signal zero-crossing ascendant declenche un grain----//
			prevsigin = sigin;
			sigin = in[xn];
			if (prevsigin <= 0 && sigin > 0) // creation d'un grain
			{
				while(--p && x->x_voiceOn[p] ) { }  // avance jusqu a la 1ere voix libre
				
				if(p)
				{
    				x->x_voiceOn[p] = 1;
					x->x_sind[p] = x->Vbeg[p] = (x->x_in2con) * t_begin[xn]*srms + (1 - x->x_in2con)* beginind;	// index dans buffer
					x->Vtranspos[p] = (x->x_in3con) * t_transpos[xn] + (1 - x->x_in3con) * x->x_transpos;			// valeur de pitch
					x->Vamp[p]		= (x->x_in4con) * t_amp[xn] + (1 - x->x_in4con) * x->x_amp;						// amplitude
					x->Vbuf[p] = (x->x_in8con) ? buffer_check(x,(int) t_sndbuf[xn] ) :  t_active_buf;		// numero du buffer son	
					x->Venv[p] = t_active_env;	// enveloppe active pour ce grain
     
					if(x->x_in5con)
					{
						if(t_length[xn]<0)
						{	
							x->Vlength[p]	= -t_length[xn]*srms;
							x->envinc[p]	= -1.*(float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= x->x_env_frames[t_active_env] - 1;
						}
						else
						{
							x->Vlength[p]	= t_length[xn]*srms;
							x->envinc[p]	= (float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= 0. ;
						}
					}
					else
					{
						x->Vlength[p] = lengthind;
						x->envinc[p] = x->x_env_dir * x->x_env_frames[t_active_env] / x->Vlength[p] ;
						if(x->x_env_dir < 0)
							x->envind[p] =  x->x_env_frames[t_active_env] - 1;
						else 
							x->envind[p] = 0.;
					}


					x->Vpan[p]		= (x->x_in6con) * t_pan[xn] + (1 - x->x_in6con) *  x->x_pan;						// pan
					x->Vdist[p]		= (x->x_in7con) * t_dist[xn] + (1 - x->x_in7con) *  x->x_dist;					// distance
					pannerV(x,p);
          
					x->x_ind[p] = 0;
					x->x_remain_ind[p] = (long) x->Vlength[p];
					x->x_delay[p] = xn;   // delay de declenchement dans le vecteur         	  
    					
				} else goto perform;
			}
			xn++ ; // boucle on incremente le pointeur dans le vecteur in et le delay de declenchement

		}
   		 
		// %%%%%%%%%%     fin creation     %%%%%%%%%%%%%%%%%
   		
   		perform :
		
		in[0] = savein;
   		//&&&&&&&&&&&&&  Boucle DSP  &&&&&&&&&&&&&
   		n = N;
   		j=0;
		while (n--)
		{
			out1[j] = 0;
			out2[j] = 0;
			j++;   
		}	
    		 	         		   				 									 
		for (i=0; i < nvoices; i++)
		{         
			//si la voix est en cours              
			if (x->x_voiceOn[i]) //&& x->x_ind[i] < x->Vlength[i] )
			{
				// si delay + grand que taille vecteur on passe ˆ voix suivante 
				if(x->x_delay[i] >= N)
				{	
					x->x_delay[i] -= N ;
					goto next;
				}
				
				// nb d'ech a caluler pour ce vecteur
				nech_process = MIN( (N - x->x_delay[i]) , x->x_remain_ind[i] );
				
				// Selon le buffer dans lequel on doit prendre le son x->Vbuf[i] (voir creation des grains)
				// Il se peut que le buffer associŽ au grain ne soit plus valide -> verification -> si non on prend buffer #0
				t_bufnum = x->x_buf_valid_index[x->Vbuf[i]];
				t_bufframes =  x->x_buf_frames[t_bufnum];
				t_bufsamples = x->x_buf_samples[t_bufnum];
				t_bufchan = x->x_buf_nchan[t_bufnum];
				
				t_envsamples = x->x_env_samples[x->Venv[i]];
				
				// pas d'increment en float
				finc = (x->Vtranspos[i]*x->x_buf_sronsr[t_bufnum]);

				// snd index source 
				source_findex = x->x_sind[i];
				// snd index cible en fin de boucle
				target_findex = source_findex + nech_process * finc;
				
				
				// nb ech a calculer different selon boucle ou pas
				if( !loop )
				{
					// si index source hors des bornes -> kill grain
					if( source_findex >  x->x_buf_frames[t_bufnum] || source_findex <  0 )
					{
						x->x_voiceOn[i] = 0;
						goto next;
					}
					
					// nb SATURE d'ech a calculer pour ce vecteur (0 < ... < frames) ou (startloop < ... < endloop)
					if(target_findex >  t_bufframes )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule
						nech_process_sat = (long)(nech_process * (t_bufframes - source_findex) /  (target_findex - source_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
					
					}else if( target_findex < 0 )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule   ATTENTION arrondi depassement
						nech_process_sat = (long)(nech_process * (source_findex) /  (source_findex - target_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
						
					}
					else 
					{
						nech_process_sat = nech_process;
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
							
					}
				}
				else // if loop
				{
					/////////// LOOOP //////////
					nech_process_sat = nech_process;
					
					//target_findex = MOD(x->x_sind[i],x->x_buf_looplength[t_bufnum]);
					
					dsp_i_begin = x->x_delay[i];
					dsp_i_end = x->x_delay[i] + nech_process_sat;
					
					loop_start  = x->x_buf_loopstart[t_bufnum];
					loop_length = x->x_buf_looplength[t_bufnum];
					
					// calcul du vrai float index en ech de lecture
					temp_sind = x->x_sind[i];
					temp_sind -= loop_start;
			        temp_sind = loop_start + MOD(temp_sind,loop_length);  
					
					// calcul de l'index de lecture buffer en long
					base_lindex = (long) temp_sind;
					lindex = interp_index_scale( temp_sind - (double) base_lindex );
					lincr  = interp_index_scale(finc);					
					

				}
				
				
				// Enveloppe long index & incr calcul
				base_lindex_env = (long) x->envind[i];
				lindex_env = interp_index_scale( x->envind[i] - (double) base_lindex_env );
				lincr_env  = interp_index_scale(x->envinc[i]);
				
				x->envind[i] += nech_process_sat * x->envinc[i];
				
				
				
				//***********************************
				// CALCUL EFFECTIF DES ECHANTILLONS
				//***********************************
				
				if(loop)
					if( x->x_sinterp == 1 )  // loop + interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
							interp_table_index = interp_get_table_index( lindex_env );
			      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
			      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= loop_start;
							buffer_index = MODI(buffer_index,loop_length);
							
							buffer_index_plus_1 = buffer_index + 1;
							//buffer_index_plus_1 = MODI(buffer_index_plus_1,x->x_buf_looplength[t_bufnum]);
							
							interp_table_index = interp_get_table_index( lindex );
			      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
			      							+ interp_table[interp_table_index].b * t_bufsamples[buffer_index_plus_1 * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan en fonction de Vhp1[i] et Vhp2[i]
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
							
							out1[j] += val1;
							out2[j] += val2;
						}
					else	// loop + non interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
			      			x->x_env[i] = t_envsamples[buffer_index];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= x->x_buf_loopstart[t_bufnum];
							buffer_index = MODI(buffer_index,x->x_buf_looplength[t_bufnum]);
							
							val = t_bufsamples[buffer_index * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan en fonction de Vhp1[i] et Vhp2[i]
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
							
							out1[j] += val1;
							out2[j] += val2;
						}
					
				else // if non loop
				if( x->x_sinterp == 1 ) // non loop + interp 
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
						interp_table_index = interp_get_table_index( lindex_env );
		      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
		      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
						interp_table_index = interp_get_table_index( lindex );
		      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
		      							+ interp_table[interp_table_index].b * t_bufsamples[(buffer_index + 1) * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan en fonction de Vhp1[i] et Vhp2[i]
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
						
						out1[j] += val1;
						out2[j] += val2;
						
						
					}
				else		// non loop + non interp
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
		      			x->x_env[i] = t_envsamples[buffer_index];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
		      			val = t_bufsamples[buffer_index * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan en fonction de Vhp1[i] et Vhp2[i]
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
						
						out1[j] += val1;
						out2[j] += val2;
						
						
					}
			
				//post("%d %d dsp_i_begin",dsp_i_begin,dsp_i_end);
				
				//********************************
				//  MAJ de l'Žtat des grains
				
				x->x_sind[i] = target_findex;
				
				x->x_ind[i] += nech_process_sat;
				
				if( (x->x_remain_ind[i] -= nech_process_sat) <= 0)
				{	
					x->x_voiceOn[i] = 0;
					//post("remain kill");
				}
				
				// decremente delai
				x->x_delay[i] = MAX(x->x_delay[i] - N,0);
					
					

			} else { 
				
				//sinon = si la voix est libre// 
	   			x->x_voiceOn[i] = 0;		
			}
			 
			next:
			j=0; //DUMMY
			   
		} // fin for
			
        		  
		x->x_sigin = sigin;

		return (w+13);

	}

	zero:
		// Pas de declenchement de grains
		while (n--)
		{
    		*out1++ = 0.0;
			*out2++ = 0.0;
		}
	
    out:    
		return (w+13);
}
//***********************************************************************************//

//-----------------------------------------------------------------------------------//
t_int *bufGranul_perform4(t_int *w)
{   

	t_bufGranul *x = (t_bufGranul *)(w[1]);

    t_float * in			= (float *)(w[2]);
	t_float * t_begin 		= (float *)(w[3]);
    t_float * t_transpos 	= (float *)(w[4]);
    t_float * t_amp			= (float *)(w[5]);
    t_float * t_length 		= (float *)(w[6]);
    t_float * t_pan 		= (float *)(w[7]);
    t_float * t_dist 		= (float *)(w[8]);
    t_float * t_sndbuf 		= (float *)(w[9]);

    t_float *out1 = (t_float *)(w[10]);
    t_float *out2 = (t_float *)(w[11]);
    t_float *out3 = (t_float *)(w[12]);
    t_float *out4 = (t_float *)(w[13]);
	
    int n = (int)(w[14]);

	int t_buf;
	
    int N = n;
    int xn = 0, p;

	double srms = x->x_sr*0.001; // double for precision
	double indT;
	float val;
	float val1,val2,val3,val4;
	float outsamp1,outsamp2;
    float sigin, prevsigin,savein; // values of actual and previous input sample   

	double beginind = x->x_begin*srms; // double for precision (this number can be very large)
	float lengthind = x->x_length*srms;	
	
	// temporary buffer index variable
	int t_active_buf ; // buffer actif pour la crŽation si pas entrŽe bufnum signal ( x->x_active_buf peut ne plus tre valide )
	int t_bufnum;
	int t_bufchan;
	int t_active_env;
	int t_envnum;
	float * t_bufsamples;
	float * t_envsamples;

    int i,j;
	int nvoices = x->x_nvoices;
	int loop = x->x_loop;
	
	int initTest;
	
	// variables grains
	int t_bufframes;
	
	float finc;
	double temp_sind;
	double source_findex, target_findex;
	int dsp_i_begin;
	int dsp_i_end;
		
	long loop_length;
	long loop_start;
	
	int nech_process, nech_process_sat;
	
		//trick interp long index
	long base_lindex;
	long lindex;
	long lincr;
	
	long base_lindex_env;
	long lindex_env;
	long lincr_env;
	
	int buffer_index;
	int buffer_index_plus_1;
	int interp_table_index;
	
	t_linear_interp * interp_table = x->x_linear_interp_table;

    
	// debug var
#ifdef PERF_DEBUG
    int n_voices_active;
#endif

   	sigin = x->x_sigin;
   	
   	
#ifdef PERF_DEBUG
	if(x->ask_poll)
	{
		n_voices_active=0;
	
		for(i=0; i< nvoices; i++) if (x->x_voiceOn[i] ) n_voices_active++;
		
		SETSYM( x->info_list, gensym("voices"));
		SETLONG(x->info_list+1, n_voices_active); 
		outlet_list(x->info,0l,2,x->info_list);
	}
#endif

	initTest = bufGranul_bufferinfos(x);
   	if(initTest == 0) goto zero;
	else
	{
		t_active_buf = buffer_check(x,x->x_active_buf);
		t_active_env = bufferenv_check(x,x->x_active_env);
		// %%%%%%%%%%     creation de grains   %%%%%%%%%%%%%%%%%
   			
   		p=nvoices;
   		
		// Dans le cas d'un grain declenche par un bang
		savein = in[0];
   		if(x->x_askfor)
   		{
			sigin = -1.;
			in[0] = 1.;
			x->x_askfor = 0;
   		}
		
		// Meme chose avec declenchement par zero-crossing sur tout le vecteur in, dans la limite fixee par nvoices
		while (n-- && p)
		{
			//-----signal zero-crossing ascendant declenche un grain----//
			prevsigin = sigin;
			sigin = in[xn];
			if (prevsigin <= 0 && sigin > 0) // creation d'un grain
			{
				while(--p && x->x_voiceOn[p] ) { }  // avance jusqu a la 1ere voix libre
				
				if(p)
				{
    				x->x_voiceOn[p] = 1;
					x->x_sind[p] = x->Vbeg[p] = (x->x_in2con) * t_begin[xn]*srms + (1 - x->x_in2con)* beginind;	// index dans buffer
					x->Vtranspos[p] = (x->x_in3con) * t_transpos[xn] + (1 - x->x_in3con) * x->x_transpos;			// valeur de pitch
					x->Vamp[p]		= (x->x_in4con) * t_amp[xn] + (1 - x->x_in4con) * x->x_amp;						// amplitude
					x->Vbuf[p] = (x->x_in8con) ? buffer_check(x,(int) t_sndbuf[xn] ) :  t_active_buf;		// numero du buffer son	
					x->Venv[p] = t_active_env;	// enveloppe active pour ce grain
     
					if(x->x_in5con)
					{
						if(t_length[xn]<0)
						{	
							x->Vlength[p]	= -t_length[xn]*srms;
							x->envinc[p]	= -1.*(float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= x->x_env_frames[t_active_env] - 1;
						}
						else
						{
							x->Vlength[p]	= t_length[xn]*srms;
							x->envinc[p]	= (float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= 0. ;
						}
					}
					else
					{
						x->Vlength[p] = lengthind;
						x->envinc[p] = x->x_env_dir * x->x_env_frames[t_active_env] / x->Vlength[p] ;
						if(x->x_env_dir < 0)
							x->envind[p] =  x->x_env_frames[t_active_env] - 1;
						else 
							x->envind[p] = 0.;
					}


					x->Vpan[p]		= (x->x_in6con) * t_pan[xn] + (1 - x->x_in6con) *  x->x_pan;						// pan
					x->Vdist[p]		= (x->x_in7con) * t_dist[xn] + (1 - x->x_in7con) *  x->x_dist;					// distance
					pannerV(x,p);
          
					x->x_ind[p] = 0;
					x->x_remain_ind[p] = (long) x->Vlength[p];
					x->x_delay[p] = xn;   // delay de declenchement dans le vecteur         	  
    					
				} else goto perform;
			}
			xn++ ; // boucle on incremente le pointeur dans le vecteur in et le delay de declenchement

		}
   		 
		// %%%%%%%%%%     fin creation     %%%%%%%%%%%%%%%%%
   		
   		perform :
		in[0] = savein;
		
   		//&&&&&&&&&&&&&  Boucle DSP  &&&&&&&&&&&&&
   		n = N;
   		j=0;
		while (n--)
		{
			out1[j] = 0;
			out2[j] = 0;
			out3[j] = 0;
			out4[j] = 0;
			j++;   
		}	
    		 	         		   				 									 
		for (i=0; i < nvoices; i++)
		{         
			//si la voix est en cours              
			if (x->x_voiceOn[i]) //&& x->x_ind[i] < x->Vlength[i] )
			{
				// si delay + grand que taille vecteur on passe ˆ voix suivante 
				if(x->x_delay[i] >= N)
				{	
					x->x_delay[i] -= N ;
					goto next;
				}
				
				// nb d'ech a caluler pour ce vecteur
				nech_process = MIN( (N - x->x_delay[i]) , x->x_remain_ind[i] );
				
				// Selon le buffer dans lequel on doit prendre le son x->Vbuf[i] (voir creation des grains)
				// Il se peut que le buffer associŽ au grain ne soit plus valide -> verification -> si non on prend buffer #0
				t_bufnum = x->x_buf_valid_index[x->Vbuf[i]];
				t_bufframes =  x->x_buf_frames[t_bufnum];
				t_bufsamples = x->x_buf_samples[t_bufnum];
				t_bufchan = x->x_buf_nchan[t_bufnum];
				
				t_envsamples = x->x_env_samples[x->Venv[i]];
				
				// pas d'increment en float
				finc = (x->Vtranspos[i]*x->x_buf_sronsr[t_bufnum]);

				// snd index source 
				source_findex = x->x_sind[i];
				// snd index cible en fin de boucle
				target_findex = source_findex + nech_process * finc;
				
				
				// nb ech a calculer different selon boucle ou pas
				if( !loop )
				{
					// si index source hors des bornes -> kill grain
					if( source_findex >  x->x_buf_frames[t_bufnum] || source_findex <  0 )
					{
						x->x_voiceOn[i] = 0;
						goto next;
					}
					
					// nb SATURE d'ech a calculer pour ce vecteur (0 < ... < frames) ou (startloop < ... < endloop)
					if(target_findex >  t_bufframes )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule
						nech_process_sat = (long)(nech_process * (t_bufframes - source_findex) /  (target_findex - source_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
					
					}else if( target_findex < 0 )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule   ATTENTION arrondi depassement
						nech_process_sat = (long)(nech_process * (source_findex) /  (source_findex - target_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
						
					}
					else 
					{
						nech_process_sat = nech_process;
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
							
					}
				}
				else // if loop
				{
					/////////// LOOOP //////////
					nech_process_sat = nech_process;
					
					//target_findex = MOD(x->x_sind[i],x->x_buf_looplength[t_bufnum]);
					
					dsp_i_begin = x->x_delay[i];
					dsp_i_end = x->x_delay[i] + nech_process_sat;
					
					loop_start  = x->x_buf_loopstart[t_bufnum];
					loop_length = x->x_buf_looplength[t_bufnum];
					
					// calcul du vrai float index en ech de lecture
					temp_sind = x->x_sind[i];
					temp_sind -= loop_start;
			        temp_sind = loop_start + MOD(temp_sind,loop_length);  
					
					// calcul de l'index de lecture buffer en long
					base_lindex = (long) temp_sind;
					lindex = interp_index_scale( temp_sind - (double) base_lindex );
					lincr  = interp_index_scale(finc);					
					

				}
				
				
				// Enveloppe long index & incr calcul
				base_lindex_env = (long) x->envind[i];
				lindex_env = interp_index_scale( x->envind[i] - (double) base_lindex_env );
				lincr_env  = interp_index_scale(x->envinc[i]);
				
				x->envind[i] += nech_process_sat * x->envinc[i];
				
				
				
				//***********************************
				// CALCUL EFFECTIF DES ECHANTILLONS
				//***********************************
				
				if(loop)
					if( x->x_sinterp == 1 )  // loop + interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
							interp_table_index = interp_get_table_index( lindex_env );
			      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
			      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= loop_start;
							buffer_index = MODI(buffer_index,loop_length);
							
							buffer_index_plus_1 = buffer_index + 1;
							//buffer_index_plus_1 = MODI(buffer_index_plus_1,x->x_buf_looplength[t_bufnum]);
							
							interp_table_index = interp_get_table_index( lindex );
			      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
			      							+ interp_table[interp_table_index].b * t_bufsamples[buffer_index_plus_1 * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan 
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
  							val3 = x->Vhp3[i]*val;
							val4 = x->Vhp4[i]*val; 
							
							out1[j] += val1;
							out2[j] += val2;
							out3[j] += val3;
							out4[j] += val4;
						}
					else	// loop + non interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
			      			x->x_env[i] = t_envsamples[buffer_index];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= x->x_buf_loopstart[t_bufnum];
							buffer_index = MODI(buffer_index,x->x_buf_looplength[t_bufnum]);
							
							val = t_bufsamples[buffer_index * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan 
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
  							val3 = x->Vhp3[i]*val;
							val4 = x->Vhp4[i]*val; 
							
							out1[j] += val1;
							out2[j] += val2;
							out3[j] += val3;
							out4[j] += val4;
						}
					
				else // if non loop
				if( x->x_sinterp == 1 ) // non loop + interp 
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
						interp_table_index = interp_get_table_index( lindex_env );
		      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
		      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
						interp_table_index = interp_get_table_index( lindex );
		      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
		      							+ interp_table[interp_table_index].b * t_bufsamples[(buffer_index + 1) * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan 
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
  						val3 = x->Vhp3[i]*val;
						val4 = x->Vhp4[i]*val; 
						
						out1[j] += val1;
						out2[j] += val2;
						out3[j] += val3;
						out4[j] += val4;
						
						
					}
				else		// non loop + non interp
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
		      			x->x_env[i] = t_envsamples[buffer_index];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
		      			val = t_bufsamples[buffer_index * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan 
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
  						val3 = x->Vhp3[i]*val;
						val4 = x->Vhp4[i]*val; 
						
						out1[j] += val1;
						out2[j] += val2;
						out3[j] += val3;
						out4[j] += val4;
						
					}
			
				//post("%d %d dsp_i_begin",dsp_i_begin,dsp_i_end);
				
				//********************************
				//  MAJ de l'Žtat des grains
				
				x->x_sind[i] = target_findex;
				
				x->x_ind[i] += nech_process_sat;
				
				if( (x->x_remain_ind[i] -= nech_process_sat) <= 0)
				{	
					x->x_voiceOn[i] = 0;
					//post("remain kill");
				}
				
				// decremente delai
				x->x_delay[i] = MAX(x->x_delay[i] - N,0);
					
					

			} else { 
				
				//sinon = si la voix est libre// 
	   			x->x_voiceOn[i] = 0;		
			}
			 
			next:
			j=0; //DUMMY
			   
		} // fin for
			
        		  
		x->x_sigin = sigin;

		return (w+15);

	}

	zero:
		// Pas de declenchement de grains
		while (n--)
		{
    		*out1++ = 0.0;
			*out2++ = 0.0;
			*out3++ = 0.0;
			*out4++ = 0.0;
		}
	
    out:    
		return (w+15);
}
//***********************************************************************************//

t_int *bufGranul_perform6(t_int *w)
{   
	t_bufGranul *x = (t_bufGranul *)(w[1]);

    t_float * in			= (float *)(w[2]);
	t_float * t_begin 		= (float *)(w[3]);
    t_float * t_transpos 	= (float *)(w[4]);
    t_float * t_amp			= (float *)(w[5]);
    t_float * t_length 		= (float *)(w[6]);
    t_float * t_pan 		= (float *)(w[7]);
    t_float * t_dist 		= (float *)(w[8]);
    t_float * t_sndbuf 		= (float *)(w[9]);

    t_float *out1 = (t_float *)(w[10]);
    t_float *out2 = (t_float *)(w[11]);
    t_float *out3 = (t_float *)(w[12]);
    t_float *out4 = (t_float *)(w[13]);
	t_float *out5 = (t_float *)(w[14]);
    t_float *out6 = (t_float *)(w[15]);
      
    int n = (int)(w[16]);

	int t_buf;
	
    int N = n;
    int xn = 0, p;

	double srms = x->x_sr*0.001; // double for precision
	double indT;
	float val;
	float val1,val2,val3,val4,val5,val6;
	float outsamp1,outsamp2;
    float sigin, prevsigin,savein; // values of actual and previous input sample   

	double beginind = x->x_begin*srms; // double for precision (this number can be very large)
	float lengthind = x->x_length*srms;	
	
	// temporary buffer index variable
	int t_active_buf ; // buffer actif pour la crŽation si pas entrŽe bufnum signal ( x->x_active_buf peut ne plus tre valide )
	int t_bufnum;
	int t_bufchan;
	int t_active_env;
	int t_envnum;
	float * t_bufsamples;
	float * t_envsamples;

    int i,j;
	int nvoices = x->x_nvoices;
	int loop = x->x_loop;
	
	int initTest;
	
	// variables grains
	int t_bufframes;
	
	float finc;
	double temp_sind;
	double source_findex, target_findex;
	int dsp_i_begin;
	int dsp_i_end;
		
	long loop_length;
	long loop_start;
	
	int nech_process, nech_process_sat;
	
		//trick interp long index
	long base_lindex;
	long lindex;
	long lincr;
	
	long base_lindex_env;
	long lindex_env;
	long lincr_env;
	
	int buffer_index;
	int buffer_index_plus_1;
	int interp_table_index;
	
	t_linear_interp * interp_table = x->x_linear_interp_table;

    
	// debug var
#ifdef PERF_DEBUG
    int n_voices_active;
#endif

   	sigin = x->x_sigin;
   	
   	
#ifdef PERF_DEBUG
	if(x->ask_poll)
	{
		n_voices_active=0;
	
		for(i=0; i< nvoices; i++) if (x->x_voiceOn[i] ) n_voices_active++;
		
		SETSYM( x->info_list, gensym("voices"));
		SETLONG(x->info_list+1, n_voices_active); 
		outlet_list(x->info,0l,2,x->info_list);
	}
#endif

	initTest = bufGranul_bufferinfos(x);
   	if(initTest == 0) goto zero;
	else
	{
		t_active_buf = buffer_check(x,x->x_active_buf);
		t_active_env = bufferenv_check(x,x->x_active_env);
		// %%%%%%%%%%     creation de grains   %%%%%%%%%%%%%%%%%
   			
   		p=nvoices;
   		
		// Dans le cas d'un grain declenche par un bang
		savein = in[0];
   		if(x->x_askfor)
   		{
			sigin = -1.;
			in[0] = 1.;
			x->x_askfor = 0;
   		}
		
		// Meme chose avec declenchement par zero-crossing sur tout le vecteur in, dans la limite fixee par nvoices
		while (n-- && p)
		{
			//-----signal zero-crossing ascendant declenche un grain----//
			prevsigin = sigin;
			sigin = in[xn];
			if (prevsigin <= 0 && sigin > 0) // creation d'un grain
			{
				while(--p && x->x_voiceOn[p] ) { }  // avance jusqu a la 1ere voix libre
				
				if(p)
				{
    				x->x_voiceOn[p] = 1;
					x->x_sind[p] = x->Vbeg[p] = (x->x_in2con) * t_begin[xn]*srms + (1 - x->x_in2con)* beginind;	// index dans buffer
					x->Vtranspos[p] = (x->x_in3con) * t_transpos[xn] + (1 - x->x_in3con) * x->x_transpos;			// valeur de pitch
					x->Vamp[p]		= (x->x_in4con) * t_amp[xn] + (1 - x->x_in4con) * x->x_amp;						// amplitude
					x->Vbuf[p] = (x->x_in8con) ? buffer_check(x,(int) t_sndbuf[xn] ) :  t_active_buf;		// numero du buffer son	
					x->Venv[p] = t_active_env;	// enveloppe active pour ce grain
     
					if(x->x_in5con)
					{
						if(t_length[xn]<0)
						{	
							x->Vlength[p]	= -t_length[xn]*srms;
							x->envinc[p]	= -1.*(float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= x->x_env_frames[t_active_env] - 1;
						}
						else
						{
							x->Vlength[p]	= t_length[xn]*srms;
							x->envinc[p]	= (float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= 0. ;
						}
					}
					else
					{
						x->Vlength[p] = lengthind;
						x->envinc[p] = x->x_env_dir * x->x_env_frames[t_active_env] / x->Vlength[p] ;
						if(x->x_env_dir < 0)
							x->envind[p] =  x->x_env_frames[t_active_env] - 1;
						else 
							x->envind[p] = 0.;
					}


					x->Vpan[p]		= (x->x_in6con) * t_pan[xn] + (1 - x->x_in6con) *  x->x_pan;						// pan
					x->Vdist[p]		= (x->x_in7con) * t_dist[xn] + (1 - x->x_in7con) *  x->x_dist;					// distance
					pannerV(x,p);
          
					x->x_ind[p] = 0;
					x->x_remain_ind[p] = (long) x->Vlength[p];
					x->x_delay[p] = xn;   // delay de declenchement dans le vecteur         	  
    					
				} else goto perform;
			}
			xn++ ; // boucle on incremente le pointeur dans le vecteur in et le delay de declenchement


		}
   		 
		// %%%%%%%%%%     fin creation     %%%%%%%%%%%%%%%%%
   		
   		perform :
   		in[0] = savein;

   		//&&&&&&&&&&&&&  Boucle DSP  &&&&&&&&&&&&&
   		n = N;
   		j=0;
		while (n--)
		{
			out1[j] = 0;
			out2[j] = 0;
			out3[j] = 0;
			out4[j] = 0;
			out5[j] = 0;
			out6[j] = 0;
			j++;   
		}	
    		 	         		   				 									 
		for (i=0; i < nvoices; i++)
		{         
			//si la voix est en cours              
			if (x->x_voiceOn[i]) //&& x->x_ind[i] < x->Vlength[i] )
			{
				// si delay + grand que taille vecteur on passe ˆ voix suivante 
				if(x->x_delay[i] >= N)
				{	
					x->x_delay[i] -= N ;
					goto next;
				}
				
				// nb d'ech a caluler pour ce vecteur
				nech_process = MIN( (N - x->x_delay[i]) , x->x_remain_ind[i] );
				
				// Selon le buffer dans lequel on doit prendre le son x->Vbuf[i] (voir creation des grains)
				// Il se peut que le buffer associŽ au grain ne soit plus valide -> verification -> si non on prend buffer #0
				t_bufnum = x->x_buf_valid_index[x->Vbuf[i]];
				t_bufframes =  x->x_buf_frames[t_bufnum];
				t_bufsamples = x->x_buf_samples[t_bufnum];
				t_bufchan = x->x_buf_nchan[t_bufnum];
				
				t_envsamples = x->x_env_samples[x->Venv[i]];
				
				// pas d'increment en float
				finc = (x->Vtranspos[i]*x->x_buf_sronsr[t_bufnum]);

				// snd index source 
				source_findex = x->x_sind[i];
				// snd index cible en fin de boucle
				target_findex = source_findex + nech_process * finc;
				
				
				// nb ech a calculer different selon boucle ou pas
				if( !loop )
				{
					// si index source hors des bornes -> kill grain
					if( source_findex >  x->x_buf_frames[t_bufnum] || source_findex <  0 )
					{
						x->x_voiceOn[i] = 0;
						goto next;
					}
					
					// nb SATURE d'ech a calculer pour ce vecteur (0 < ... < frames) ou (startloop < ... < endloop)
					if(target_findex >  t_bufframes )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule
						nech_process_sat = (long)(nech_process * (t_bufframes - source_findex) /  (target_findex - source_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
					
					}else if( target_findex < 0 )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule   ATTENTION arrondi depassement
						nech_process_sat = (long)(nech_process * (source_findex) /  (source_findex - target_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
						
					}
					else 
					{
						nech_process_sat = nech_process;
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
							
					}
				}
				else // if loop
				{
					/////////// LOOOP //////////
					nech_process_sat = nech_process;
					
					//target_findex = MOD(x->x_sind[i],x->x_buf_looplength[t_bufnum]);
					
					dsp_i_begin = x->x_delay[i];
					dsp_i_end = x->x_delay[i] + nech_process_sat;
					
					loop_start  = x->x_buf_loopstart[t_bufnum];
					loop_length = x->x_buf_looplength[t_bufnum];
					
					// calcul du vrai float index en ech de lecture
					temp_sind = x->x_sind[i];
					temp_sind -= loop_start;
			        temp_sind = loop_start + MOD(temp_sind,loop_length);  
					
					// calcul de l'index de lecture buffer en long
					base_lindex = (long) temp_sind;
					lindex = interp_index_scale( temp_sind - (double) base_lindex );
					lincr  = interp_index_scale(finc);					
					

				}
				
				
				// Enveloppe long index & incr calcul
				base_lindex_env = (long) x->envind[i];
				lindex_env = interp_index_scale( x->envind[i] - (double) base_lindex_env );
				lincr_env  = interp_index_scale(x->envinc[i]);
				
				x->envind[i] += nech_process_sat * x->envinc[i];
				
				
				
				//***********************************
				// CALCUL EFFECTIF DES ECHANTILLONS
				//***********************************
				
				if(loop)
					if( x->x_sinterp == 1 )  // loop + interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
							interp_table_index = interp_get_table_index( lindex_env );
			      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
			      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= loop_start;
							buffer_index = MODI(buffer_index,loop_length);
							
							buffer_index_plus_1 = buffer_index + 1;
							//buffer_index_plus_1 = MODI(buffer_index_plus_1,x->x_buf_looplength[t_bufnum]);
							
							interp_table_index = interp_get_table_index( lindex );
			      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
			      							+ interp_table[interp_table_index].b * t_bufsamples[buffer_index_plus_1 * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan 
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
  							val3 = x->Vhp3[i]*val;
							val4 = x->Vhp4[i]*val;
							val5 = x->Vhp5[i]*val;
							val6 = x->Vhp6[i]*val;
							
							out1[j] += val1;
							out2[j] += val2;
							out3[j] += val3;
							out4[j] += val4;
							out5[j] += val5;
							out6[j] += val6;
						}
					else	// loop + non interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
			      			x->x_env[i] = t_envsamples[buffer_index];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= x->x_buf_loopstart[t_bufnum];
							buffer_index = MODI(buffer_index,x->x_buf_looplength[t_bufnum]);
							
							val = t_bufsamples[buffer_index * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan 
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
  							val3 = x->Vhp3[i]*val;
							val4 = x->Vhp4[i]*val;
							val5 = x->Vhp5[i]*val;
							val6 = x->Vhp6[i]*val;
							
							out1[j] += val1;
							out2[j] += val2;
							out3[j] += val3;
							out4[j] += val4;
							out5[j] += val5;
							out6[j] += val6;
						}
					
				else // if non loop
				if( x->x_sinterp == 1 ) // non loop + interp 
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
						interp_table_index = interp_get_table_index( lindex_env );
		      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
		      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
						interp_table_index = interp_get_table_index( lindex );
		      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
		      							+ interp_table[interp_table_index].b * t_bufsamples[(buffer_index + 1) * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan 
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
						val3 = x->Vhp3[i]*val;
						val4 = x->Vhp4[i]*val;
						val5 = x->Vhp5[i]*val;
						val6 = x->Vhp6[i]*val;
						
						out1[j] += val1;
						out2[j] += val2;
						out3[j] += val3;
						out4[j] += val4;
						out5[j] += val5;
						out6[j] += val6;
						
						
					}
				else		// non loop + non interp
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
		      			x->x_env[i] = t_envsamples[buffer_index];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
		      			val = t_bufsamples[buffer_index * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan 
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
						val3 = x->Vhp3[i]*val;
						val4 = x->Vhp4[i]*val;
						val5 = x->Vhp5[i]*val;
						val6 = x->Vhp6[i]*val;
						
						out1[j] += val1;
						out2[j] += val2;
						out3[j] += val3;
						out4[j] += val4;
						out5[j] += val5;
						out6[j] += val6;
						
					}
			
				//post("%d %d dsp_i_begin",dsp_i_begin,dsp_i_end);
				
				//********************************
				//  MAJ de l'Žtat des grains
				
				x->x_sind[i] = target_findex;
				
				x->x_ind[i] += nech_process_sat;
				
				if( (x->x_remain_ind[i] -= nech_process_sat) <= 0)
				{	
					x->x_voiceOn[i] = 0;
					//post("remain kill");
				}
				
				// decremente delai
				x->x_delay[i] = MAX(x->x_delay[i] - N,0);
					
					

			} else { 
				
				//sinon = si la voix est libre// 
	   			x->x_voiceOn[i] = 0;		
			}
			 
			next:
			j=0; //DUMMY
			   
		} // fin for
			
        		  
		x->x_sigin = sigin;

		return (w+17);

	}

	zero:
		// Pas de declenchement de grains
		while (n--)
		{
    		*out1++ = 0.0;
			*out2++ = 0.0;
			*out3++ = 0.0;
			*out4++ = 0.0;
			*out5++ = 0.0;
			*out6++ = 0.0;
		}
	
    out:    
		return (w+17);
}

//***********************************************************************************//

t_int *bufGranul_perform8(t_int *w)
{   
	t_bufGranul *x = (t_bufGranul *)(w[1]);

    t_float * in			= (float *)(w[2]);
	t_float * t_begin 		= (float *)(w[3]);
    t_float * t_transpos 	= (float *)(w[4]);
    t_float * t_amp			= (float *)(w[5]);
    t_float * t_length 		= (float *)(w[6]);
    t_float * t_pan 		= (float *)(w[7]);
    t_float * t_dist 		= (float *)(w[8]);
    t_float * t_sndbuf 		= (float *)(w[9]);

    t_float *out1 = (t_float *)(w[10]);
    t_float *out2 = (t_float *)(w[11]);
    t_float *out3 = (t_float *)(w[12]);
    t_float *out4 = (t_float *)(w[13]);
	t_float *out5 = (t_float *)(w[14]);
    t_float *out6 = (t_float *)(w[15]);
	t_float *out7 = (t_float *)(w[16]);
    t_float *out8 = (t_float *)(w[17]);
      
    int n = (int)(w[18]);

	int t_buf;
	
    int N = n;
    int xn = 0, p;

	double srms = x->x_sr*0.001; // double for precision
	double indT;
	float val;
	float val1,val2,val3,val4,val5,val6,val7,val8;
	float outsamp1,outsamp2;
    float sigin, prevsigin,savein; // values of actual and previous input sample   

	double beginind = x->x_begin*srms; // double for precision (this number can be very large)
	float lengthind = x->x_length*srms;	
	
	// temporary buffer index variable
	int t_active_buf ; // buffer actif pour la crŽation si pas entrŽe bufnum signal ( x->x_active_buf peut ne plus tre valide )
	int t_bufnum;
	int t_bufchan;
	int t_active_env;
	int t_envnum;
	float * t_bufsamples;
	float * t_envsamples;

    int i,j;
	int nvoices = x->x_nvoices;
	int loop = x->x_loop;
	
	int initTest;
	
	// variables grains
	int t_bufframes;
	
	float finc;
	double temp_sind;
	double source_findex, target_findex;
	int dsp_i_begin;
	int dsp_i_end;
		
	long loop_length;
	long loop_start;
	
	int nech_process, nech_process_sat;
	
		//trick interp long index
	long base_lindex;
	long lindex;
	long lincr;
	
	long base_lindex_env;
	long lindex_env;
	long lincr_env;
	
	int buffer_index;
	int buffer_index_plus_1;
	int interp_table_index;
	
	t_linear_interp * interp_table = x->x_linear_interp_table;

    
	// debug var
#ifdef PERF_DEBUG
    int n_voices_active;
#endif

   	sigin = x->x_sigin;
   	
   	
#ifdef PERF_DEBUG
	if(x->ask_poll)
	{
		n_voices_active=0;
	
		for(i=0; i< nvoices; i++) if (x->x_voiceOn[i] ) n_voices_active++;
		
		SETSYM( x->info_list, gensym("voices"));
		SETLONG(x->info_list+1, n_voices_active); 
		outlet_list(x->info,0l,2,x->info_list);
	}
#endif

	initTest = bufGranul_bufferinfos(x);
   	if(initTest == 0) goto zero;
	else
	{
		t_active_buf = buffer_check(x,x->x_active_buf);
		t_active_env = bufferenv_check(x,x->x_active_env);
		// %%%%%%%%%%     creation de grains   %%%%%%%%%%%%%%%%%
   			
   		p=nvoices;
   		
		// Dans le cas d'un grain declenche par un bang
		savein = in[0];
   		if(x->x_askfor)
   		{
			sigin = -1.;
			in[0] = 1.;
			x->x_askfor = 0;
   		}
		
		// Meme chose avec declenchement par zero-crossing sur tout le vecteur in, dans la limite fixee par nvoices
		while (n-- && p)
		{
			//-----signal zero-crossing ascendant declenche un grain----//
			prevsigin = sigin;
			sigin = in[xn];
			if (prevsigin <= 0 && sigin > 0) // creation d'un grain
			{
				while(--p && x->x_voiceOn[p] ) { }  // avance jusqu a la 1ere voix libre
				
				if(p)
				{
    				x->x_voiceOn[p] = 1;
					x->x_sind[p] = x->Vbeg[p] = (x->x_in2con) * t_begin[xn]*srms + (1 - x->x_in2con)* beginind;	// index dans buffer
					x->Vtranspos[p] = (x->x_in3con) * t_transpos[xn] + (1 - x->x_in3con) * x->x_transpos;			// valeur de pitch
					x->Vamp[p]		= (x->x_in4con) * t_amp[xn] + (1 - x->x_in4con) * x->x_amp;						// amplitude
					x->Vbuf[p] = (x->x_in8con) ? buffer_check(x,(int) t_sndbuf[xn] ) :  t_active_buf;		// numero du buffer son	
					x->Venv[p] = t_active_env;	// enveloppe active pour ce grain
     
					if(x->x_in5con)
					{
						if(t_length[xn]<0)
						{	
							x->Vlength[p]	= -t_length[xn]*srms;
							x->envinc[p]	= -1.*(float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= x->x_env_frames[t_active_env] - 1;
						}
						else
						{
							x->Vlength[p]	= t_length[xn]*srms;
							x->envinc[p]	= (float)(x->x_env_frames[t_active_env] - 1.) / x->Vlength[p] ;
							x->envind[p]	= 0. ;
						}
					}
					else
					{
						x->Vlength[p] = lengthind;
						x->envinc[p] = x->x_env_dir * x->x_env_frames[t_active_env] / x->Vlength[p] ;
						if(x->x_env_dir < 0)
							x->envind[p] =  x->x_env_frames[t_active_env] - 1;
						else 
							x->envind[p] = 0.;
					}


					x->Vpan[p]		= (x->x_in6con) * t_pan[xn] + (1 - x->x_in6con) *  x->x_pan;						// pan
					x->Vdist[p]		= (x->x_in7con) * t_dist[xn] + (1 - x->x_in7con) *  x->x_dist;					// distance
					pannerV(x,p);
          
					x->x_ind[p] = 0;
					x->x_remain_ind[p] = (long) x->Vlength[p];
					x->x_delay[p] = xn;   // delay de declenchement dans le vecteur         	  
    					
				} else goto perform;
			}
			xn++ ; // boucle on incremente le pointeur dans le vecteur in et le delay de declenchement

		}
		// %%%%%%%%%%     fin creation     %%%%%%%%%%%%%%%%%
   		
   		perform :
		in[0] = savein;
		
   		//&&&&&&&&&&&&&  Boucle DSP  &&&&&&&&&&&&&
   		n = N;
   		j=0;
		while (n--)
		{
			out1[j] = 0;
			out2[j] = 0;
			out3[j] = 0;
			out4[j] = 0;
			out5[j] = 0;
			out6[j] = 0;
			out7[j] = 0;
			out8[j] = 0;
			j++;   
		}	
    		 	         		   				 									 
		for (i=0; i < nvoices; i++)
		{         
			//si la voix est en cours              
			if (x->x_voiceOn[i]) //&& x->x_ind[i] < x->Vlength[i] )
			{
				// si delay + grand que taille vecteur on passe ˆ voix suivante 
				if(x->x_delay[i] >= N)
				{	
					x->x_delay[i] -= N ;
					goto next;
				}
				
				// nb d'ech a caluler pour ce vecteur
				nech_process = MIN( (N - x->x_delay[i]) , x->x_remain_ind[i] );
				
				// Selon le buffer dans lequel on doit prendre le son x->Vbuf[i] (voir creation des grains)
				// Il se peut que le buffer associŽ au grain ne soit plus valide -> verification -> si non on prend buffer #0
				t_bufnum = x->x_buf_valid_index[x->Vbuf[i]];
				t_bufframes =  x->x_buf_frames[t_bufnum];
				t_bufsamples = x->x_buf_samples[t_bufnum];
				t_bufchan = x->x_buf_nchan[t_bufnum];
				
				t_envsamples = x->x_env_samples[x->Venv[i]];
				
				// pas d'increment en float
				finc = (x->Vtranspos[i]*x->x_buf_sronsr[t_bufnum]);

				// snd index source 
				source_findex = x->x_sind[i];
				// snd index cible en fin de boucle
				target_findex = source_findex + nech_process * finc;
				
				
				// nb ech a calculer different selon boucle ou pas
				if( !loop )
				{
					// si index source hors des bornes -> kill grain
					if( source_findex >  x->x_buf_frames[t_bufnum] || source_findex <  0 )
					{
						x->x_voiceOn[i] = 0;
						goto next;
					}
					
					// nb SATURE d'ech a calculer pour ce vecteur (0 < ... < frames) ou (startloop < ... < endloop)
					if(target_findex >  t_bufframes )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule
						nech_process_sat = (long)(nech_process * (t_bufframes - source_findex) /  (target_findex - source_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
					
					}else if( target_findex < 0 )
					{
						// on met la voix off
						x->x_voiceOn[i] = 0;
						// jusqu'a quel index on calcule   ATTENTION arrondi depassement
						nech_process_sat = (long)(nech_process * (source_findex) /  (source_findex - target_findex));
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
						
					}
					else 
					{
						nech_process_sat = nech_process;
						
						dsp_i_begin = x->x_delay[i];
						dsp_i_end = x->x_delay[i] + nech_process_sat;
						
						temp_sind = x->x_sind[i];
						base_lindex = (long) x->x_sind[i];
						lindex = interp_index_scale( x->x_sind[i] - (double) base_lindex );
						lincr  = interp_index_scale(finc);
							
					}
				}
				else // if loop
				{
					/////////// LOOOP //////////
					nech_process_sat = nech_process;
					
					//target_findex = MOD(x->x_sind[i],x->x_buf_looplength[t_bufnum]);
					
					dsp_i_begin = x->x_delay[i];
					dsp_i_end = x->x_delay[i] + nech_process_sat;
					
					loop_start  = x->x_buf_loopstart[t_bufnum];
					loop_length = x->x_buf_looplength[t_bufnum];
					
					// calcul du vrai float index en ech de lecture
					temp_sind = x->x_sind[i];
					temp_sind -= loop_start;
			        temp_sind = loop_start + MOD(temp_sind,loop_length);  
					
					// calcul de l'index de lecture buffer en long
					base_lindex = (long) temp_sind;
					lindex = interp_index_scale( temp_sind - (double) base_lindex );
					lincr  = interp_index_scale(finc);					
					

				}
				
				
				// Enveloppe long index & incr calcul
				base_lindex_env = (long) x->envind[i];
				lindex_env = interp_index_scale( x->envind[i] - (double) base_lindex_env );
				lincr_env  = interp_index_scale(x->envinc[i]);
				
				x->envind[i] += nech_process_sat * x->envinc[i];
				
				
				
				//***********************************
				// CALCUL EFFECTIF DES ECHANTILLONS
				//***********************************
				
				if(loop)
					if( x->x_sinterp == 1 )  // loop + interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
							interp_table_index = interp_get_table_index( lindex_env );
			      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
			      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= loop_start;
							buffer_index = MODI(buffer_index,loop_length);
							
							buffer_index_plus_1 = buffer_index + 1;
							//buffer_index_plus_1 = MODI(buffer_index_plus_1,x->x_buf_looplength[t_bufnum]);
							
							interp_table_index = interp_get_table_index( lindex );
			      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
			      							+ interp_table[interp_table_index].b * t_bufsamples[buffer_index_plus_1 * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan 
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
							val3 = x->Vhp3[i]*val;
							val4 = x->Vhp4[i]*val;
							val5 = x->Vhp5[i]*val;
							val6 = x->Vhp6[i]*val;
							val7 = x->Vhp7[i]*val;
							val8 = x->Vhp8[i]*val;
							
							out1[j] += val1;
							out2[j] += val2;
							out3[j] += val3;
							out4[j] += val4;
							out5[j] += val5;
							out6[j] += val6;
							out7[j] += val7;
							out8[j] += val8;
						}
					else	// loop + non interp
						for(j= dsp_i_begin; j < dsp_i_end; j++)
						{
						// Lecture de l'enveloppe
							buffer_index = base_lindex_env + interp_get_int( lindex_env );
			      			x->x_env[i] = t_envsamples[buffer_index];
			      			
			      			lindex_env += lincr_env;
			      			
			      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
							
							// Lecture de la forme d'onde
				    	  	buffer_index = base_lindex + interp_get_int( lindex );
				    	  	buffer_index -= x->x_buf_loopstart[t_bufnum];
							buffer_index = MODI(buffer_index,x->x_buf_looplength[t_bufnum]);
							
							val = t_bufsamples[buffer_index * t_bufchan];
			      			
			      			lindex += lincr;
			      			
							// calcul de la valeur env[i]*son[i]*amp[i]
							val = x->x_env[i]*val*x->Vamp[i]; 

							// calcul du pan 
							val1 = x->Vhp1[i]*val;
							val2 = x->Vhp2[i]*val; 
							val3 = x->Vhp3[i]*val;
							val4 = x->Vhp4[i]*val;
							val5 = x->Vhp5[i]*val;
							val6 = x->Vhp6[i]*val;
							val7 = x->Vhp7[i]*val;
							val8 = x->Vhp8[i]*val;
							
							out1[j] += val1;
							out2[j] += val2;
							out3[j] += val3;
							out4[j] += val4;
							out5[j] += val5;
							out6[j] += val6;
							out7[j] += val7;
							out8[j] += val8;
						}
					
				else // if non loop
				if( x->x_sinterp == 1 ) // non loop + interp 
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
						interp_table_index = interp_get_table_index( lindex_env );
		      			x->x_env[i] = interp_table[interp_table_index].a * t_envsamples[buffer_index]
		      							+ interp_table[interp_table_index].b * t_envsamples[buffer_index + 1];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
						interp_table_index = interp_get_table_index( lindex );
		      			val = interp_table[interp_table_index].a * t_bufsamples[buffer_index * t_bufchan]
		      							+ interp_table[interp_table_index].b * t_bufsamples[(buffer_index + 1) * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan 
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
						val3 = x->Vhp3[i]*val;
						val4 = x->Vhp4[i]*val;
						val5 = x->Vhp5[i]*val;
						val6 = x->Vhp6[i]*val;
						val7 = x->Vhp7[i]*val;
						val8 = x->Vhp8[i]*val;
						
						out1[j] += val1;
						out2[j] += val2;
						out3[j] += val3;
						out4[j] += val4;
						out5[j] += val5;
						out6[j] += val6;
						out7[j] += val7;
						out8[j] += val8;
						
						
					}
				else		// non loop + non interp
					for(j= dsp_i_begin; j < dsp_i_end; j++)
					{
					
						// Lecture de l'enveloppe
						buffer_index = base_lindex_env + interp_get_int( lindex_env );
		      			x->x_env[i] = t_envsamples[buffer_index];
		      			
		      			lindex_env += lincr_env;
		      			
		      			//x->envind[i] += x->envinc[i]; // 2 REMOVE
						
						// Lecture de la forme d'onde
			    	  	buffer_index = base_lindex + interp_get_int( lindex );
		      			val = t_bufsamples[buffer_index * t_bufchan];
		      			
		      			lindex += lincr;

						// calcul de la valeur env[i]*son[i]*amp[i]
						val = x->x_env[i]*val*x->Vamp[i]; 

						// calcul du pan 
						val1 = x->Vhp1[i]*val;
						val2 = x->Vhp2[i]*val; 
						val3 = x->Vhp3[i]*val;
						val4 = x->Vhp4[i]*val;
						val5 = x->Vhp5[i]*val;
						val6 = x->Vhp6[i]*val;
						val7 = x->Vhp7[i]*val;
						val8 = x->Vhp8[i]*val;
						
						out1[j] += val1;
						out2[j] += val2;
						out3[j] += val3;
						out4[j] += val4;
						out5[j] += val5;
						out6[j] += val6;
						out7[j] += val7;
						out8[j] += val8;
						
					}
			
				//post("%d %d dsp_i_begin",dsp_i_begin,dsp_i_end);
				
				//********************************
				//  MAJ de l'Žtat des grains
				
				x->x_sind[i] = target_findex;
				
				x->x_ind[i] += nech_process_sat;
				
				if( (x->x_remain_ind[i] -= nech_process_sat) <= 0)
				{	
					x->x_voiceOn[i] = 0;
					//post("remain kill");
				}
				
				// decremente delai
				x->x_delay[i] = MAX(x->x_delay[i] - N,0);
					
					

			} else { 
				
				//sinon = si la voix est libre// 
	   			x->x_voiceOn[i] = 0;		
			}
			 
			next:
			j=0; //DUMMY
			   
		} // fin for
			
        		  
		x->x_sigin = sigin;

		return (w+19);

	}

	zero:
		// Pas de declenchement de grains
		while (n--)
		{
    		*out1++ = 0.0;
			*out2++ = 0.0;
			*out3++ = 0.0;
			*out4++ = 0.0;
			*out5++ = 0.0;
			*out6++ = 0.0;
			*out7++ = 0.0;
			*out8++ = 0.0;
		}
	
    out:    
		return (w+19);
}

//***********************************************************************************//