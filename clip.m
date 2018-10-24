function [ cliped_frame ] = clip( frame )

fs=8e3;
CL=0.65*max(abs(frame));
for i=1:length(frame)
    if (frame(i))>CL
        cliped_frame(i)=frame(i)-CL;
    elseif (frame(i))<-CL
        cliped_frame(i)=frame(i)+CL;
    else
        cliped_frame(i)=0;
    end;
end;

end

