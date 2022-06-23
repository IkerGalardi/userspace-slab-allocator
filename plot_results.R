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
        xlab("Operation") + 
        ylab("Time (ns)") +
        theme(text = element_text(size = 8))

p2 = ggplot(a10b10, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("α = 10, β = 10") +
        xlab("Operation") + 
        ylab("Time (ns)") + 
        theme(text = element_text(size = 8))


p3 = ggplot(a20b2, aes(x=name, y=value)) + 
        geom_bar(stat = "identity") +
        ggtitle("α = 20, β = 2") +
        xlab("Operation") +
        ylab("Time (ns)") +
        theme(text = element_text(size = 8))


p4 = plot_grid(p1, p2, p3, labels=c("", "", ""), ncol=2, nrow=2)
sizedistrfile = "sizedistr.pdf"
save_plot(sizedistrfile, p4, ncol=2, nrow=2, base_asp=1.1)

