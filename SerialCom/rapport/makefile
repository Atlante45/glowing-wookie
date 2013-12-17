TEX=pdflatex
PDF=rapport.pdf 

SRC=$(wildcard *.tex) 
all: $(PDF) 

rapport.pdf: rapport.tex $(SRC) $(IMG)
	$(TEX) $<
	$(TEX) $<
	rm *.aux *.log *.toc *.out

.PHONY: clean display

display:
	evince $(PDF) &

clean:
	rm *.aux *.log *.toc *.pdf *.out

