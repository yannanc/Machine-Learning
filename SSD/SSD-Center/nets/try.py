import tensorflow as tf

params = tf.constant([[1,2,3,4],[5,6,7,8]])
indices = tf.argmax(params,axis=1)
indices = tf.reshape(indices, [2,1])
row = tf.range(indices.shape[0])[:,None]
row = tf.cast(row, tf.int64)
ind = tf.concat([row, indices],axis=1)
getpar = tf.gather_nd(params,ind)

with tf.Session() as sess:
    ind = sess.run(ind)
    print(ind)
    x = sess.run(getpar)
    print(x)