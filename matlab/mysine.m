function y = mysine(f,fs,N)
y = zeros(N, 1);
M = 4096*1000;
wt = sin(linspace(0, pi, M));

theta = 1;
w=single(int32(f * M * 2 / fs));
for i = 1:N
    if(theta>M)
       y(i) = -wt(theta-M);
    else
       y(i) = wt(theta);
    end
    theta=theta+w;
    if(theta > 2*M)
       theta = theta - 2*M; 
    end
end