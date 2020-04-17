
from keras.callbacks import Callback

#save model
class save_model(Callback):
    def __init__(self, model0):
        self.model = model0

    def on_epoch_end(self, epoch, logs={}):
        print("epoch %d completed .."%epoch)
        if ((epoch+1) % 1) == 0:
            #val_loss = logs['val_loss']
            loss=logs['loss']
            modelfile = "model_loss-%.3f_%d_.hdf5"%(loss,epoch)
            #modelfile = 'model-ep{epoch:03d}-loss{loss:.3f}-val_loss{val_loss:.3f}.h5'
            #modelfile =  "weights.{epoch:02d}-{val_loss:.2f}.hdf5"
            self.model.save(modelfile)
        #ModelCheckpoint(filepath='/tmp/newweightsi_{epoch:02d}.hdf5', monitor='acc',verbose=1, period=1 ,save_best_only=False,mode='max')
        f = open("checkpoint","w+")
        f1 = open("checkpoint.log","a")
        f1.write(modelfile)
        f1.write("\n")
        f.write(modelfile)
        f.close()
        f1.close()