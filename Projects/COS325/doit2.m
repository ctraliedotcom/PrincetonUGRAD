%freqs and dbs gives the harmonic "fingerprint" of an instrument, maybe from Audacity or some other
%frequency analyzer
freqs = [446 870 1328 1765 2203 2652 3092];
dbs = [-10, -27, -20, -21, -30, -20, -29];
mags = 1000 * db2mag(dbs); %Convert to a magnitude from a decibel, and scale by 1000 to avoid underflow
freqs = freqs';%take the transposes
mags = mags';

%matrixfreqs = [ones(7,1) freqs' ones(7,1).*1000 freqs'./(2^(2/12)) 2000.*ones(7,1) freqs'./(2^(4/12)) 3000.*ones(7,1) freqs'/(2^(2/12)) 4000.*ones(7,1) freqs' 5000.*ones(7,1) freqs'.*(2^(3/12)) 6000.*ones(7,1) freqs'.*(2^(5/12))];
%matrixamps = [ones(7,1) mags' 1000.*ones(7,1) mags' 2000.*ones(7,1) mags'
%3000.*ones(7,1) mags' 4000.*ones(7,1) mags' 5000.*ones(7,1) mags'
%6000.*ones(7,1) mags'];

numnotes = 15;
matrixfreqs = [];
matrixamps = [];
notes = [0 -12 -0 12 24 36 24 12 0 -12 -24 36 0 -36 0]; %put this on a logarithmic scale
%this is an array of halfsteps to jump up or down from the base note defined by freqs and dbs
for i=1:numnotes
    time = ((1000)*(i-1) + 1);
    freqs.*2^((notes(i)/12));
    factor = 2^(notes(i)/12);
    matrixfreqs = [matrixfreqs ones(7,1).*time freqs.*factor];
    matrixamps = [matrixamps ones(7,1).*time mags];
end

%matrixfreqs
%matrixamps

finalmatrix = [];

for i = 1:7
    finalmatrix = [finalmatrix; matrixamps(i:i, 1:numnotes*2) ; matrixfreqs(i:i, 1:numnotes*2)];
end

%finalmatrix

dlmwrite('out.txt', finalmatrix, ' ');%delimit matrix elements with a space
signal = addSyn('out.txt', [0, 0], 8800, 1, 1);
wavwrite(signal, 8800, 'ctralie2.wav');