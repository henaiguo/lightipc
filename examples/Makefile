all:
	@for i in `ls Makefile.*` ; do $(MAKE) -s -f $$i || exit 1; done
	
clean:
	@echo clean
	@for i in `ls Makefile.*` ; do $(MAKE) -s -f $$i clean || exit 1; done

