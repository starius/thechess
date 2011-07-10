
countdown2:
	wget -r -l 1 -A '*.zip' http://plugins.jquery.com/project/countdown2
	unzip plugins.jquery.com/files/jquery.countdown*.zip -d countdown2
	rm -r plugins.jquery.com/

build: countdown2

install: build
	mkdir -p /usr/share/javascript/jquery/countdown2
	cp countdown2/* /usr/share/javascript/jquery/countdown2/

clean:
	rm -r countdown2/

