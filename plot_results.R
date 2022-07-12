library(cowplot)
library(ggplot2)

a2b20 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(72873162, 103571327, 50730320, 52936917)
)

a10b10 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(79901282, 99504954, 86121991, 100169212)
)

a20b2 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(62541873, 104819043, 65699973, 107080553)
)

p1 = ggplot(a2b20, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("α = 2, β = 20") +
  xlab("") + 
  ylab("Time (ns)") +
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))

p2 = ggplot(a10b10, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("α = 10, β = 10") +
  xlab("") + 
  ylab("Time (ns)") + 
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))


p3 = ggplot(a20b2, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("α = 20, β = 2") +
  xlab("") +
  ylab("Time (ns)") +
  theme_classic() +
  theme(plot.title = element_text(hjust = 0.5))


p4 = plot_grid(p1, p2, p3, labels=c("", "", ""), ncol=2, nrow=2)
sizedistrfile = "sizedistr.pdf"
save_plot(sizedistrfile, p4, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)


d20 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(1686355545, 1089208619, 4706860645, 1864357995)
)

d200 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(1685756779, 1107479466, 6596187449, 1950222742)
)

d2000 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(1695083546, 1144882002, 6636479421, 1975314440)
)

p5 = ggplot(d20, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("d = 20") +
  xlab("") + 
  ylab("Time (ns)") +
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))

p6 = ggplot(d200, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("d = 200") +
  xlab("") + 
  ylab("Time (ns)") + 
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))


p7 = ggplot(d2000, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("d = 2000") +
  xlab("") +
  ylab("Time (ns)") +
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))

p8 = plot_grid(p5, p6, p7, labels=c("", "", ""), ncol=2, nrow=2)
randompatternfname = "randomapttern.pdf"
save_plot(randompatternfname, p8, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)

cfrac = data.frame(
  name=c("glibc alloctor", "slab allocator"),
  value=c(1.099, 1.096)
)

p9 = ggplot(cfrac, aes(x=name, y=value)) + 
  geom_bar(stat = "identity", colour="#87a4cd", fill="#dae8fc") +
  ggtitle("CFrac") +
  xlab("") +
  ylab("Time (s)") +
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))
cfracfname = "cfrac.pdf"
save_plot(cfracfname, p9, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)

distancevol = data.frame(
  x =       c(        20,        200,       2000,      20000,      200000,     2000000),
  alloc =   c(4028085636, 7501593222, 8354511011, 8512508232,  8510238906,  8816088437),
  dealloc = c(1827426859, 1932027530, 2703255631, 5264387499, 12315686735, 17798934838)
)

p10 = ggplot(distancevol, aes(x=x, y=alloc)) + 
  geom_line(colour="#7b9ac7") +
  ggtitle("Distance evolution (allocations)") +
  xlab("Average distance") +
  ylab("Time (ns)") +
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))

p11 = ggplot(distancevol, aes(x=x, y=dealloc)) + 
  geom_line(colour="#7b9ac7") +
  ggtitle("Distance evolution (allocations)") +
  xlab("Average distance") +
  ylab("Time (ns)") +
  theme_classic() + 
  theme(plot.title = element_text(hjust = 0.5))

p12 = plot_grid(p10, p11, labels=c("", ""), ncol=2, nrow=1)
save_plot("distancevol.pdf", p12, ncol=2, nrow=1, base_asp=1.1, device=cairo_pdf)
