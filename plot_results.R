library(cowplot)
library(ggplot2)

a2b20 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(73762004, 105279871, 48850355, 53842230)
)

a10b10 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(80618721, 100255538, 85237269, 100701580)
)

a20b2 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(63419295, 104708128, 64289456, 106009778)
)

p1 = ggplot(a2b20, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("α = 2, β = 20") +
        xlab("") + 
        ylab("Time (ns)") +
        theme_classic()

p2 = ggplot(a10b10, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("α = 10, β = 10") +
        xlab("") + 
        ylab("Time (ns)") + 
        theme_classic()


p3 = ggplot(a20b2, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("α = 20, β = 2") +
        xlab("") +
        ylab("Time (ns)") +
        theme_classic()


p4 = plot_grid(p1, p2, p3, labels=c("", "", ""), ncol=2, nrow=2)
sizedistrfile = "sizedistr.pdf"
save_plot(sizedistrfile, p4, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)


d20 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(1353739496, 1302657973, 1296069239, 1811195904)
)

d200 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(1562964912, 1372038624, 2205603636, 4090888374)
)

d2000 = data.frame(
  name=c("glibc alloc", "glibc free", "slab alloc", "slab free"),
  value=c(1459956050, 1299646708, 1324878552, 1819149237)
)

p5 = ggplot(d20, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("d = 20") +
        xlab("") + 
        ylab("Time (ns)") +
        theme_classic()

p6 = ggplot(d200, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("d = 200") +
        xlab("") + 
        ylab("Time (ns)") + 
        theme_classic()


p7 = ggplot(d2000, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("d = 2000") +
        xlab("") +
        ylab("Time (ns)") +
        theme_classic()

p8 = plot_grid(p5, p6, p7, labels=c("", "", ""), ncol=2, nrow=2)
randompatternfname = "randomapttern.pdf"
save_plot(randompatternfname, p8, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)

cfrac = data.frame(
  name=c("glibc alloctor", "slab allocator"),
  value=c(1.098, 1.097)
)

p9 = ggplot(cfrac, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("CFrac") +
        xlab("") +
        ylab("Time (s)") +
        theme_classic()
cfracfname = "cfrac.pdf"
save_plot(cfracfname, p9, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)
