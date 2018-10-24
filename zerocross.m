function [ zerocross_num ] = zerocross( frame )

frame_check=(1:length(frame)-1);
shifted_frame=frame(2:length(frame));
zerocross_num=sum(sign(frame_check).*sign(shifted_frame)<=0);
zerocross_num=zerocross_num-sum(zerocross_num==0);

end

