# [Random_Magic_Number](http://www.ict.griffith.edu.au/anthony/info/C/RandomNumbers)	

	-----------------------------------------------------------------------------
		  "Efficient and Portable Combined Random Number Generators",
		   Pierre L'Ecuyer, Communications of the ACM, June, 1988 (v31 #6)
		
		  "Random Number Generators: Good Ones Are Hard To Find",
		   Park, Stephen K., and Miller, Keith W.,
		   Communications of the ACM, October, 1988.
		   (see below)
		
		Conclusions :- horrible random (sic) number generators are used by people
		  who don't know better, while very good ones take less than 20 lines of
		  Pascal!  Amoung the horrid generators are some that come with certain
		  systems or are presented in textbooks!
		
		R. Salfi: *COMPSTAT 1974*. Vienna: 1974, pp 28 - 35...
		  If the outcome of a random generator is solely based on the value of the
		  previous outcome, like a linear congruential method, then the outcome of
		  a shuffle depends on exactly three things: the shuffling algorithm, the
		  input and the seed of the random generator. Hence, for a given list and a
		  given algorithm, the outcome of the shuffle is purely based on the seed.
		
		  Many modern computers have 32 bit random numbers, hence a 32 bit seed.
		  Hence, there are at most 2^32 possible shuffles of a list, foreach of the
		  possible algorithms. But for a list of n elements, there are n! possible
		  permutations. Which means that a shuffle of a list of 13 elements will not
		  generate certain permutations, as 13! > 2^32.
		
		-----------------------------------------------------------------------------
		White Noise Random Number Generators...
		
		I believe it was in Andrew S. Tanenbaum's book "Computer Networks" that a
		great random number generator was proposed - amplify the kT noise in a
		resistor, sample the voltage every so often, and digitize it.  You'll get a
		nice random sequence...
		                              ---  David F. Skoll   dfs@doe.carleton.ca
		
		I read some years ago about some people who tried to do this as part of some
		research project, and carefully analyzed the results. Their conclusion was
		that it is just about impossible to get flat distribution (white noise). All
		noise in nature is either colored one way or another, or else the noise is
		not, but the *measuring* instruments interfere with sampling in such a way as
		to give a colored distribution again.
		                              --- Doug Merritt   doug@eris.berkeley.edu
		
		Incidentally, most noise-based generators can be used in combination with
		computed pseudo-random sequences, by XORing a sample from each source.  It
		should be pretty hard to find patterns after that.
		                              --- Norman Diamond  diamond@jit081.enet.dec.com
		
		Another way (which is equivalent) is to have a precise function generator
		generating sawtooth waves, which signal is then sampled (at arbitrary time)
		with an A/D converter.  The randomness is introduced in the time domain of
		the sampling.
		                              --- John Porter  1292az@gmuvax2.gmu.edu
		
		-----------------------------------------------------------------------------
		Simple (bad) Psuedo Random Number Generator (Sic)
		The low bit typically just toggles between calls.
		
		random() {
		   seed = ( seed * mulitiplier + increment ) % modulus;
		   return seed;
		}
		
		Table of Good values
		   Multiplier    Increment     Modulus
		      25173         13849        65536
		       9301         49297       233280
		
		-----------------------------------------------------------------------------
		/*
		** Dr. Park's algorithm published in the Oct. '88 ACM
		** "Random Number Generators: Good Ones Are Hard To Find"
		** His version available at ftp://cs.wm.edu/pub/rngs.tar
		** This is called a   Lehmer Generator
		*/
		
		static long Seed = 1;       /* This is required to be 32 bits long */
		
		long random()
		/*
		**    Random number between  0  and  2147483647 (2**31 - 1)  inclusive
		**/
		{
		  a = 48271
		  m = 2147483647
		  r = 3399    /* r = m mod a */
		  q = 44488   /* q = m div a */
		  if ((Seed = Seed % q * a - Seed / q * r) < 0)
		    Seed += m;
		  return Seed - 1;
		}
		/* From  xlockmore  utils.c */
		
		--- Or in pascal ----
		
		function Random : real;
		{ Initialize seed with 1..2147483646 }
		{ maxint must be greater than or equal to 2**31 - 1 }
		const
		  a = 16807;
		  m = 2147483647;
		  q = 12773;       { q = m div a }
		  r = 2836;        { r = m mod a }
		var
		  lo, hi, test : integer;
		begin
		  hi := seed div q;
		  lo := seed mod q;
		  test := a * lo - r * hi;
		  if test > 0 then
		    seed := test
		  else
		    seed := test + m;
		  Random := seed / m;   { make floating point - important }
		end;
		
		--- Seeds ---
		
		  Seeds you can use (calculate q and r as above)
		     a = 48271    m = 2147483647
		         16807        2147483647
		
		-----------------------------------------------------------------------------
		Here is the Portable Combined Generator of L'Ecuyer for 32-bit computers.
		It has a period of roughly 8.12544 x 10**12.
		EG: two psudo-random generators working together.
		
		s1 : int := seed
		
		Function Uniform : real;
		{ Initialize s1 to 1..2147483562; s2 to 1..2147483398 }
		var
		  Z, k : integer;
		begin
		  k  := s1 div 53668;
		  s1 := 40014 * (s1 - k * 53668) - k * 12211;
		  if s1 < 0 then s1 := s1 + 2147483563;
		
		  k  := s2 div 52774;
		  s2 := 40692 * (s2 - k * 52774) - k * 3791;
		  if s2 < 0 then s2 := s2 + 2147483399;
		
		  Z := s1 - s2;
		  if Z < 1 then Z := Z + 2147483562;
		
		  Uniform := Z * 4.656613e-10;
		end;
		
		-------------------------------------------------------------------------------