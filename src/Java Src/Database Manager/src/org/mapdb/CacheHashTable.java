/*
 *  Copyright (c) 2012 Jan Kotek
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package org.mapdb;

/**
 * Fixed size cache which uses hash table.
 * Is thread-safe and requires only minimal locking.
 * Items are randomly removed and replaced by hash collisions.
 * <p/>
 * This is simple, concurrent, small-overhead, random cache.
 *
 * @author Jan Kotek
 */
public class CacheHashTable extends EngineWrapper implements Engine {


    protected final Locks.RecidLocks locks = new Locks.SegmentedRecidLocks(16);

    protected HashItem[] items;
    protected final int cacheMaxSize;

    private static class HashItem {
        final long key;
        final Object val;

        private HashItem(long key, Object val) {
            this.key = key;
            this.val = val;
        }
    }



    public CacheHashTable(Engine engine, int cacheMaxSize) {
        super(engine);
        this.items = new HashItem[cacheMaxSize];
        this.cacheMaxSize = cacheMaxSize;
    }

    @Override
    public <A> long put(A value, Serializer<A> serializer) {
        final long recid = engine.put(value, serializer);
        final int pos = position(recid);
        try{
            locks.lock(pos);
            items[position(recid)] = new HashItem(recid, value);
        }finally{
            locks.unlock(pos);
        }
        return recid;
    }

    @Override
    @SuppressWarnings("unchecked")
    public <A> A get(long recid, Serializer<A> serializer) {
        final int pos = position(recid);
        HashItem item = items[pos];
        if(item!=null && recid == item.key)
            return (A) item.val;

        try{
            locks.lock(pos);
            //not in cache, fetch and add
            final A value = engine.get(recid, serializer);
            if(value!=null)
                items[pos] = new HashItem(recid, value);
            return value;
        }finally{
            locks.unlock(pos);
        }
    }

    private int position(long recid) {
        return Math.abs(Utils.longHash(recid))%cacheMaxSize;
    }

    @Override
    public <A> void update(long recid, A value, Serializer<A> serializer) {
        final int pos = position(recid);
        try{
            locks.lock(pos);
            items[pos] = new HashItem(recid, value);
            engine.update(recid, value, serializer);
        }finally {
            locks.unlock(pos);
        }
    }

    @Override
    public <A> boolean compareAndSwap(long recid, A expectedOldValue, A newValue, Serializer<A> serializer) {
        final int pos = position(recid);
        try{
            locks.lock(pos);
            HashItem item = items[pos];
            if(item!=null && item.key == recid){
                if(item.val == null && expectedOldValue!=null) return false;
                //found in cache, so compare values
                if(item.val == expectedOldValue || item.val.equals(expectedOldValue)){
                    //found matching entry in cache, so just update and return true
                    items[pos] = new HashItem(recid, newValue);
                    engine.update(recid, newValue, serializer);
                    return true;
                }else{
                    return false;
                }
            }else{
                boolean ret = engine.compareAndSwap(recid, expectedOldValue, newValue, serializer);
                if(ret) items[pos] = new HashItem(recid, newValue);
                return ret;
            }
        }finally {
            locks.unlock(pos);
        }
    }

    @Override
    public void delete(long recid) {
        final int pos = position(recid);
        try{
            locks.lock(recid);
            engine.delete(recid);
            HashItem item = items[pos];
            if(item!=null && recid == item.key)
            items[pos] = null;
        }finally {
            locks.unlock(recid);
        }

}


    @Override
    public void close() {
        engine.close();
        //dereference to prevent memory leaks
        engine = null;
        items = null;
    }

    @Override
    public void rollback() {
        for(int i = 0;i<items.length;i++)
            items[i] = null;
        engine.rollback();
    }


}
