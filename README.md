This repo hosts my enthusisatic code experiments.
It's meant for my personal consumption, so haters go away.

The code is patchy and will seem a bit random to the casual observer.
It's certainly NOT meant for anything other than journalling my thoughts around ideas.

The process in my own mind is throw everything in one place.
Use any method that gets the job done.
Document facts and opinions in commits.

Some seed thoughts (code) currently in the repo are:

* OtherEndian is a UDT meant to transparently define wire endianess for comms / storage protocols.
  This class implements the normal arithmetic operators to allow transparent conversion and interaction with native types.
  Native endianess is determined at build time, with the intention of allowing no-cost abstraction of the dominant little endian encoding without penalty.
* FixedPoint is a UDT meant to bridge the gap between floating point types and q-types (fixed precision integrals).
  The hope was that Q arithmetic could calculated at build time to ensure the underlying integrals used for the calculations were of sufficient width.
  This was to entail precalculating arithmetic shifts and masks based on the underlying Q M.N type.
  It quickly became bigger than Ben Hur and was TBH a little beyond my ability.
  Nevertheless, there is a need to improve current best practice which is to capture the Q M.N structure in parameter names
  and MANUALLY perform the required arithmetic shifts, which is highly error prone and not detectable by the compiler.
