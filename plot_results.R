library(cowplot)
library(ggplot2)

randomalloc.plot_single = function (name, distance) {
  smallocname = paste(name, "_smalloc.csv", sep = "")
  systemname  = paste(name, "_sys.csv", sep = "")
  
  smalloccsv = read.csv(smallocname, header = TRUE)
  systemcsv = read.csv(systemname, header = TRUE)
  
  data = data.frame(
    name  = c("glibc alloc", "glibc free", "slab alloc", "slab free"),
    value = c(mean(systemcsv[,1]), mean(systemcsv[,2]), mean(smalloccsv[,1]), mean(smalloccsv[,2])),
    sd    = c(sd(systemcsv[,1]), sd(systemcsv[,2]), sd(smalloccsv[,1]), sd(smalloccsv[,2]))
  )
  
  title = paste("d = ", distance)
  
  ggplot(data) + 
    geom_bar(aes(x=name, y=value), stat = "identity", colour="#87a4cd", fill="#dae8fc") +
    geom_errorbar( aes(x=name, ymin=value-sd, ymax=value+sd), width=0.2, colour="#9673a6", size=0.5) +
    ggtitle(title) +
    xlab("") + 
    ylab("Time (ns)") +
    theme_classic() + 
    theme(plot.title = element_text(hjust = 0.5))
}

randomalloc.plot_all = function(output_name) {
  plot.d20    = randomalloc.plot_single("bench_d20",       20)
  plot.d200   = randomalloc.plot_single("bench_d200",     200)
  plot.d2000  = randomalloc.plot_single("bench_d2000",   2000)
  plot.d20000 = randomalloc.plot_single("bench_d20000", 20000)
  
  plot.all = plot_grid(plot.d20, plot.d200, plot.d2000, plot.d20000, labels=c("", "", "", ""), ncol=2, nrow=2)
  save_plot(output_name, plot.all, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)
}

randomsize.plot_single = function (name, alfa, beta) {
  smallocname = paste(name, "_smalloc.csv", sep = "")
  systemname  = paste(name, "_sys.csv", sep = "")
  
  smalloccsv = read.csv(smallocname, header = TRUE)
  systemcsv = read.csv(systemname, header = TRUE)
  
  data = data.frame(
    name  = c("glibc alloc", "glibc free", "slab alloc", "slab free"),
    value = c(mean(systemcsv[,1]), mean(systemcsv[,2]), mean(smalloccsv[,1]), mean(smalloccsv[,2])),
    sd    = c(sd(systemcsv[,1]), sd(systemcsv[,2]), sd(smalloccsv[,1]), sd(smalloccsv[,2]))
  )
  
  title = paste("α = ", alfa, ", β = ", beta)
  
  ggplot(data) + 
    geom_bar(aes(x=name, y=value), stat = "identity", colour="#87a4cd", fill="#dae8fc") +
    geom_errorbar( aes(x=name, ymin=value-sd, ymax=value+sd), width=0.2, colour="#9673a6", size=0.5) +
    ggtitle(title) +
    xlab("") + 
    ylab("Time (ns)") +
    theme_classic() + 
    theme(plot.title = element_text(hjust = 0.5))
}

randomsize.plot_all = function(output_name) {
  plot.a2b20  = randomsize.plot_single("bench_a2b20",   2, 20)
  plot.a10b10 = randomsize.plot_single("bench_a10b10", 10, 10)
  plot.a20b2  = randomsize.plot_single("bench_a20b2",  20,  2)
  
  plot.all = plot_grid(plot.a2b20, plot.a10b10, plot.a20b2, labels=c("", "", ""), ncol=2, nrow=2)
  save_plot(output_name, plot.all, ncol=2, nrow=2, base_asp=1.1, device=cairo_pdf)
}

cfracbench.plot = function(output_name) {
  smalloccsv = read.csv("bench_cfrac_smalloc.csv", header = FALSE)
  systemcsv = read.csv("bench_cfrac_sys.csv", header = FALSE)
  
  data = data.frame(
    name  = c("glibc allocator", "slab allocator"),
    value = c(mean(systemcsv[,1]), mean(smalloccsv[,1])),
    sd    = c(sd(systemcsv[,1]), sd(smalloccsv[,1]))
  )
  
  plot.int = ggplot(data) + 
    geom_bar(aes(x=name, y=value), stat = "identity", colour="#87a4cd", fill="#dae8fc") +
    geom_errorbar( aes(x=name, ymin=value-sd, ymax=value+sd), width=0.2, colour="#9673a6", size=0.5) +
    ggtitle("") +
    xlab("") + 
    ylab("Time (s)") +
    theme_classic() + 
    theme(plot.title = element_text(hjust = 0.5))
  
  save_plot(output_name, plot.int, ncol=1, nrow=1, base_asp=1.1, device=cairo_pdf)
}

randomsize.plot_all("sizedistr.pdf")
randomalloc.plot_all("randomapttern.pdf")
cfracbench.plot("cfrac.pdf")