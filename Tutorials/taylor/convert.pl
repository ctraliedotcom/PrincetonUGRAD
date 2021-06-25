$contents = getfile($ARGV[0]);

@array = split(/<latex>/, $contents);
open(FILEOUT, ">$ARGV[1]");
$pngindex = 0;
print FILEOUT shift(@array);

foreach $a(@array) {
	@latex = split(/<\/latex>/, $a);
	#$latex[0] holds the latex command
	$lateximage = &writeLatex($latex[0], $pngindex);
	print FILEOUT "<img src = \"$lateximage\" alt = \"$latex[0]\">";
	print FILEOUT $latex[1];
	$pngindex++;
}

sub getfile {
	my $filename = shift;
	open F, "< $filename" or die "Couldn't open `$filename': $!";
	my $contents;
	{ local $/ = undef;     # Read entire file at once
	$contents = <F>;      # Return file as one single `line'
	}                       # $/ regains its old value
	close F;
	return $contents;
}

sub writeLatex {
	my $str = "\\documentclass{article}\n\\usepackage{amsmath}\n\\usepackage{amsthm}\n\\usepackage{amssymb}\n\\usepackage{bm}\n\\newcommand{\\mx}[1]{\\mathbf{\\bm{#1}}} % Matrix command\n\\newcommand{\\vc}[1]{\\mathbf{\\bm{#1}}} % Vector command\n\\newcommand{\\T}{\\text{T}} % Transpose\n\\pagestyle{empty}\n\\begin{document}\n$_[0]\n\\end{document}";
	#my $str = "\\documentclass{article}\n\\begin{document}\n\\begin{equation}\n$_[0]\n\\end{equation}\n\\end{document}";
	my $filename = "latex$_[1].png";
	$latexfile = "equation";
	open(EQFILE, ">$latexfile.tex");
	print EQFILE $str;
	close(EQFILE);
	`latex $latexfile.tex`;
	`dvipng -T tight -x 1500 -z 9 -bg Transparent -o $filename $latexfile.dvi`;
	`del equation*`;
	$filename;
}