TARGETS=line_bresenham line_sampling line_supersampling line_sdf line_sdfaabb
OUTPUTS=$(addsuffix .png, $(TARGETS))
ZOOM=$(addsuffix _zoom.png, $(TARGETS))
CROP=$(addsuffix _crop.png, $(TARGETS))

all: $(TARGETS) stitchheart
test: $(TARGETS) $(OUTPUTS) stitchheart.png
zoom: $(TARGETS) $(ZOOM)
montage: $(TARGETS) line_montage_zoom.png

%: %.c
	gcc -Wall -O3 -o $@ $<

%.png: %
	time ./$<

%_zoom.png: %.png
	convert $< -shave 192 -filter point -resize 400% $@

%_crop.png: %.png
	convert $< -shave 192 $@

line_montage_zoom.png: $(CROP)
	montage -label '%t' $(CROP) -geometry '1x1+2+2<' -tile x1 $@

clean:
	rm $(TARGETS) *.png
