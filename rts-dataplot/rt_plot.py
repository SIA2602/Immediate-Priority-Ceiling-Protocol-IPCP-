########################################################################################
# Esse script eh feito para as atividades de escalonamento em tempo real
# Voce deve executar o teu codigo do epos como abaixo:
# $ make APPLICATION=rt_rm run > rt_rm_output
# Isso ira gerar um arquivo com a saida da aplicacao de teste fornecida pelo professor.
# O mesmo pode ser feito com EDF
# Olhe a main deste codigo la em baixo. Eh bem intuitiva.
# --------------------------------------------------------------------------------------
# Autor: Lucas de Camargo Souza
########################################################################################
import matplotlib.pyplot as plt

ignore = ['M','A','B','C', 'D']
colors = ['r.', 'b.', 'g.', 'k.', 'y.', 'c.', 'm.', 'r.', 'b.', 'g.', 'k.', 'y.', 'c.', 'm.', 'r.', 'b.', 'g.', 'k.', 'y.', 'c.', 'm.']

class OutputData:
    def __init__(self):
        self.data = []

    def getData(self):
        x = []
        Y = []
        i = 0
        c = []
        for d in self.data:
            if d[1] in ignore:
                pass

            else:
                if d[1] in [e[0] for e in c]:
                    for e in c:
                        if d[1] == e[0]:
                            x[e[1]].append(d[0])
                            Y[e[1]].append(Y[e[1]][0][1])

                else:
                    c.append((d[1],i))
                    x.append([])
                    Y.append([])
                    Y[i].append((d[1],i))
                    Y[i].append(Y[i][0][1])
                    x[i].append(d[0])
                    i += 1
        
        return x,Y

    def exportFile(self, fname, mode=0):
        if(mode == 0):
            x,y = self.getData()
            with open(fname, 'w') as f:
                f.write(str(x))
                f.write('\n')
                f.write(str(y))
        elif(mode == 1):
            with open(fname, 'w') as f:
                f.write(str(self.data))

# Use esta classe para um arquivo de saida do tipo:
# 0 C
# 1 C
# 2 A
# ...
class OutputFile(OutputData):
    def __init__(self,fname):
        super().__init__()
        self.fname = fname
        self.crop()

    def crop(self):
        with open(self.fname,'r') as f:
            while(True):
                line = f.readline()
                
                if(line == ""):
                    break

                lnumber,sep,temp = line.partition("  ")
                if(len(temp)>1):
                    thread,sep,temp = temp.partition("  ")
                else:
                    thread = temp

                try:
                    if lnumber.isdigit():
                        self.data.append((int(lnumber),thread[0] if thread[0].isalpha() else int(thread)))
                except:
                    print("Could not append ",lnumber,"; ",thread)



# Use esta classe para um arquivo de saida puro e.g.
# make APPLICATION=rt_edf run > arquivo_saida_puro
class OutputRawFile(OutputData):
    def __init__(self, fname):
        super().__init__()
        self.fname = fname
        self.crop()

    def crop(self):
        with open(self.fname,'r') as f:
            while(True):
                line = f.readline()
                
                if(line == ""):
                    print("Inconsistent File!")
                    raise EOFError

                if(line.find("Threads will now be created and I'll wait for them to finish...") != -1):
                    break

            f.readline()

            while(True):
                line = f.readline()

                if(line == ""):
                    print("Inconsistent File!")
                    raise EOFError
                
                if(line.find("... done!") != -1):
                    break

                lnumber,sep,temp = line.partition('\t')
                thread,sep,temp = temp.partition('\t')
                try:
                    if lnumber.isdigit():
                        self.data.append((int(lnumber),thread[0] if thread[0].isalpha() else int(thread)))
                except:
                    print("Could not append ",lnumber,"; ",thread)


class MatPlotter:
    def __init__(self):
        self.data = []
        plt.rcParams.update({'font.size': 20})
        plt.rcParams['figure.constrained_layout.use'] = True

    def add_data(self, data, label : str = ""):
        self.data.append((data,label))

    def plot(self, x_i=0, x_s=0):
        if(len(self.data) ==  1):
            if(x_s != 0):
                plt.xlim(x_i,x_s)
            
            yt1 = ()
            yt2 = ()
            for i in range(len(self.data[0][0][1])):
                plt.plot(self.data[0][0][0][i],self.data[0][0][1][i][1:], colors[i])
                yt1 += (self.data[0][0][1][i][0][1],)
                yt2 += (self.data[0][0][1][i][0][0],)

                proc = []
                for p in self.data[0][0][1][i][1:]:
                    proc.append(-1)

                plt.plot(self.data[0][0][0][i],proc, colors[i])
                

            plt.ylabel(self.data[0][1])
            plt.grid(b=True, which='major', color='#909090', linestyle='-')
            plt.minorticks_on()
            plt.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

            yt1 += (-1,)
            yt2 += ('P',)

            plt.yticks(yt1,yt2)

        elif(len(self.data) ==  2):     
            f, (ax1, ax2) = plt.subplots(2, 1)

            yt1 = ()
            yt2 = ()
            for i in range(len(self.data[0][0][1])):
                ax1.plot(self.data[0][0][0][i],self.data[0][0][1][i][1:], colors[i])
                yt1 += (self.data[0][0][1][i][0][1],)
                yt2 += (self.data[0][0][1][i][0][0],)

                proc = []
                for p in self.data[0][0][1][i][1:]:
                    proc.append(-1)

                ax1.plot(self.data[0][0][0][i],proc, colors[i])

            yt1 += (-1,)
            yt2 += ('P',)
            plt.sca(ax1)
            plt.yticks(yt1,yt2)

            ax1.set_ylabel(self.data[0][1])
            ax1.grid(b=True, which='major', color='#909090', linestyle='-')
            ax1.minorticks_on()
            ax1.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)

            yt1 = ()
            yt2 = ()
            for i in range(len(self.data[1][0][1])):
                ax2.plot(self.data[1][0][0][i],self.data[1][0][1][i][1:], colors[i])
                yt1 += (self.data[1][0][1][i][0][1],)
                yt2 += (self.data[1][0][1][i][0][0],)

                proc = []
                for p in self.data[1][0][1][i][1:]:
                    proc.append(-1)

                ax2.plot(self.data[1][0][0][i],proc, colors[i])

            yt1 += (-1,)
            yt2 += ('P',)
            plt.sca(ax2)
            plt.yticks(yt1,yt2)

            ax2.set_ylabel(self.data[1][1])
            ax2.grid(b=True, which='major', color='#909090', linestyle='-')
            ax2.minorticks_on()
            ax2.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.3)

            if(x_s != 0):
                ax1.set_xlim(x_i,x_s) 
                ax2.set_xlim(x_i,x_s)

        MatPlotter.maximize()

    def show(self):
        plt.show()

    @staticmethod
    def maximize():
        plot_backend = plt.get_backend()
        mng = plt.get_current_fig_manager()
        if plot_backend == 'TkAgg':
            mng.resize(*mng.window.maxsize())
        elif plot_backend == 'wxAgg':
            mng.frame.Maximize(True)
        elif plot_backend == 'Qt4Agg':
            mng.window.showMaximized()
        elif plot_backend == 'Qt5Agg':
            mng.window.showMaximized()

if __name__=="__main__":    
    f1 = OutputRawFile("polling_server_output")
    f2 = OutputRawFile("rt_rm_output")

    plot = MatPlotter()

    # Voce pode dicionar um ou dois arquivos no mesmo grafico.
    # Nao implementei pra mais que 2!
    # plot.add_data(f1.getData(),"Polling Server")
    plot.add_data(f2.getData(),"Rate Monotonic")
    
    plot.plot(0,2500)
    plot.show()
